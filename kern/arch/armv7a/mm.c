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
#include <list.h>
#include <util.h>
#include <mm.h>
#include <mmu.h>
#include <pmm.h>
#include <vmm.h>

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
 */
void page_index_clear(pgindex_t * index)
{
	arm_pte_l1_t *page_table = index;
	int i;
	for (i = 0; i < ARM_PT_L1_LENGTH; ++i) {
		page_table[i] = 0;
	}
}

/* This internal routine does not check for bad parameters */
static inline void __arm_map_sect(arm_pte_l1_t *page_table, addr_t paddr,
	size_t vaddr, uint32_t ap, uint32_t dom)
{
	arm_pte_l1_t entry = ARM_PT_L1_SECT;
	entry |= paddr >> ARM_SECT_SHIFT << 20;
	entry |= ap << 10;
	entry |= dom << 5;
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
		__arm_map_sect(index, paddr, vaddr, ARM_PT_AP_USER_NONE, 0);
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
	extern uint8_t _kern_end;
	struct pages *p = kmalloc(sizeof(struct pages), 0);
	p->paddr = (addr_t)premap_addr((size_t)&_kern_end);
	p->size = get_mem_size() - ((addr_t)(size_t)(&_kern_end) - KERN_BASE);
	p->flags = 0;
	free_pages(p);
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

