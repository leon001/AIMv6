/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
 *
 * This file is part of AIMv6.
 *
 * AIMv6 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AIMv6 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

/* from kernel */
#include <sys/types.h>
#include <aim/export.h>
#include <list.h>
#include <util.h>
#include <mm.h>
#include <mmu.h>
#include <pmm.h>
#include <vmm.h>
#include <panic.h>
#include <libc/string.h>

static struct allocator_cache *pt_l1_cache = NULL, *pt_l2_cache = NULL;

addr_t get_mem_physbase()
{
	return RAM_PHYSBASE;
}

addr_t get_mem_size()
{
	/* FIXME */
	extern void *RAM_SIZE;
	return (addr_t)(size_t)&RAM_SIZE;
}

/*
 * by CLEARing a page index, caller assumes it contains no allocated space.
 * Often used to initialize a page index.
 * Two functions for clearing two different levels of the page table.
 * First one used by early boot stages, and both are used as allocator
 * callbacks, so they are named differently and have different signature.
 */
void page_index_clear(pgindex_t * index)
{
	memset(index, 0, ARM_PT_L1_SIZE);
}

static void __pt_l2_clear(void *pt)
{
	memset(pt, 0, ARM_PT_L2_SIZE);
}

/* process general flags into ARM-specific flags */
#define mkflags(flags, ap, dom, tex, c, b, s, xn) \
	do { \
	uint32_t f = (flags); \
	if (f & VMA_WRITE) (ap) = ARM_PT_AP_USER_BOTH; \
	else if (f & VMA_READ) (ap) = ARM_PT_AP_USER_READ; \
	else (ap) = ARM_PT_AP_USER_NONE; \
	(dom) = 0; \
	if (f & VMA_EXEC) (xn) = 0; \
	else (xn) = 1; \
	f &= MAP_TYPE_MASK; \
	switch(f) { \
		case MAP_USER_MEM: (tex)=1; (c)=1; (b)=1; (s)=1; break; \
		case MAP_KERN_MEM: (tex)=1; (c)=1; (b)=1; (s)=1; break; \
		case MAP_PRIV_DEV: (tex)=2; (c)=0; (b)=0; (s)=0; break; \
		case MAP_SHARED_DEV: (tex)=0; (c)=0; (b)=1; (s)=0; break; \
		default: (tex)=1; (c)=1; (b)=1; (s)=1; \
	} \
	} while (0)

/* internal routines does not check for bad parameters */
static inline void __arm_map_sect(arm_pte_l1_t *page_table, addr_t paddr,
	size_t vaddr, uint32_t flags)
{
	uint32_t ap, dom, tex, c, b, s, xn;
	/* process flags */
	mkflags(flags, ap, dom, tex, c, b, s, xn);

	/* apply map */
	arm_pte_l1_t entry = ARM_PT_L1_SECT;
	entry |= paddr;
	entry |= s << 16;
	entry |= tex << 12;
	entry |= ap << 10;
	entry |= dom << 5;
	entry |= xn << 4;
	entry |= c << 3;
	entry |= b << 2;
	page_table[vaddr >> ARM_SECT_SHIFT] = entry;
}

/*
 * Early map routine does not try to allocate memory.
 */
int page_index_early_map(pgindex_t *index, addr_t paddr, size_t vaddr,
	size_t length)
{
	/* alignment check */
	if (IS_ALIGNED(paddr, ARM_SECT_SIZE) &&
	    IS_ALIGNED(vaddr, ARM_SECT_SIZE) &&
	    IS_ALIGNED(length, ARM_SECT_SIZE) != 1) {
		return EOF;
	}
	/* map each ARM SECT */
	size_t vend = vaddr + length;
	while (vaddr < vend) {
		__arm_map_sect(index, paddr, vaddr, MAP_SHARED_DEV | VMA_EXEC);
		paddr += ARM_SECT_SIZE;
		vaddr += ARM_SECT_SIZE;
	}
	return 0;
}

/*
 * mmu_init()
 * initialize the MMU with given page index
 */
int mmu_init(pgindex_t *index)
{
    asm volatile (
        /* Address */
        "mcr     p15, 0, %[index], c2, c0, 0;"
        /* access permission */
        "mov     r0, #0x1;"
        "mcr     p15, 0, r0, c3, c0, 0;"
        /* turn on MMU */
        "mrc     p15, 0, r0, c1, c0, 0;"
        "orr     r0, r0, #0x1;"
        "mcr     p15, 0, r0, c1, c0, 0;"
        ::
        /* For ARM cores, we have low address now, don't translate address. */
        [index] "r" (index)
    );
    return 0;
}

/* add memory chunks to page allocator */
void add_memory_pages(void)
{
	extern uint8_t SYMBOL(kern_end);
	struct pages pages = {
		.paddr = (addr_t)premap_addr((size_t)&SYMBOL(kern_end)),
		.size = get_mem_size() -
			((addr_t)(size_t)(&SYMBOL(kern_end)) - KERN_BASE),
		.flags = 0
	};
	free_pages(&pages);
}

/* get_addr_space()
 * determine whether we are running in low address or in high address
 * return values:
 * 0 - low address
 * 1 - high address
 * negative - reserved for errors
 */
int get_addr_space()
{
	uint32_t pc;

	asm volatile (
		"mov	%[pc], pc"
		:[pc] "=r" (pc)
	);
	return (pc > KERN_BASE);
}

void arch_mm_init(void)
{
	/* initialize allocator cache for L1 page tables */
	pt_l1_cache = kmalloc(sizeof(*pt_l1_cache), 0);
	assert(pt_l1_cache != NULL);
	pt_l1_cache->size = ARM_PT_L1_SIZE;
	pt_l1_cache->align = ARM_PT_L1_SIZE;
	pt_l1_cache->flags = 0;
	pt_l1_cache->create_obj = (void *)page_index_clear;
	pt_l1_cache->destroy_obj = (void *)page_index_clear;
	assert(cache_create(pt_l1_cache) == 0);

	/* initialize allocator cache for L2 page tables */
	pt_l2_cache = kmalloc(sizeof(*pt_l2_cache), 0);
	assert(pt_l2_cache != NULL);
	pt_l2_cache->size = ARM_PT_L2_SIZE;
	pt_l2_cache->align = ARM_PT_L2_SIZE;
	pt_l2_cache->flags = 0;
	pt_l2_cache->create_obj = __pt_l2_clear;
	pt_l2_cache->destroy_obj = __pt_l2_clear;
	assert(cache_create(pt_l2_cache) == 0);

	/* SCU, cache and branch predict goes here */
}

pgindex_t *init_pgindex(void)
{
	return (pgindex_t *)cache_alloc(pt_l1_cache);
}

void destroy_pgindex(pgindex_t *pgindex)
{
	arm_pte_l1_t *table = pgindex;
	int i;

	/* free L2 tables (if any) */
	for (i = 0; i < ARM_PT_L1_LENGTH; i += 1) {
		uint32_t type = table[i] & ARM_PT_L1_TYPE_MASK;
		if (type == ARM_PT_L1_TABLE) {
			uint32_t l2table =
				table[i] & ARM_PT_L1_TABLE_BASE_MASK;
			cache_free(pt_l2_cache, (void *)l2table);
		}
	}

	/* free the L1 table itself */
	cache_free(pt_l1_cache, pgindex);
}

int map_pages(pgindex_t *pgindex, void *vaddr, addr_t paddr, size_t size,
    uint32_t flags)
{
	uint32_t ap, dom, tex, c, b;
	/* access flags */
	dom = 0;

	/* try to map some sections */
	
	return 0;
}

ssize_t unmap_pages(pgindex_t *pgindex, void *vaddr, size_t size, addr_t *paddr)
{
	return 0;
}

