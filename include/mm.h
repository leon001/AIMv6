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

#ifndef _MM_H
#define _MM_H

#include <mmu.h>
#include <pmm.h>
#include <sys/types.h>

/* premap_addr: always returns low address.
 * The function which assumes that the argument is a high address
 * becomes __premap_addr(). */
#define premap_addr(a)	({ \
	size_t i = (size_t)(a); \
	(i >= KERN_BASE) ? __premap_addr(i) : i; \
})

/* postmap_addr: always returns high address.
 * The function which assumes that the argument is a low address
 * becomes __postmap_addr(). */
#define postmap_addr(a)	({ \
	size_t i = (size_t)(a); \
	(i >= KERN_BASE) ? (i) : __postmap_addr(i); \
})

#ifndef __ASSEMBLER__

addr_t get_mem_physbase();
addr_t get_mem_size();

/*
 * Data structure to hold early mappings.
 * type indicates how the mapping should be treated after we
 * jump up to kernel address space.
 * EARLY_MAPPING_MEMORY - Nothing will be done.
 * EARLY_MAPPING_KMMAP - Will be translated to an ioremap() result.
 */
struct early_mapping {
	addr_t	paddr;
	size_t	vaddr;
	size_t	size;
	int	type;
};
#define	EARLY_MAPPING_MEMORY	0
#define EARLY_MAPPING_KMMAP	1
#define EARLY_MAPPING_TEMP	2
#define EARLY_MAPPING_OTHER	3

void early_mapping_clear(void);
size_t early_mapping_add_memory(addr_t base, size_t size);
size_t early_mapping_add_kmmap(addr_t base, size_t size);
int early_mapping_add(struct early_mapping *entry);
struct early_mapping *early_mapping_next(struct early_mapping *base);

int page_index_init(pgindex_t *boot_page_index);
int mmu_init(pgindex_t *boot_page_index);

void early_mm_init(void);	/* arch-specific */

/* Clear all MMU init callback handlers */
void mmu_handlers_clear(void);
/* Add one MMU init callback handler, which will be called *after*
 * initializing MMU*/
int mmu_handlers_add(generic_fp entry);
void mmu_handlers_apply(void);

/* Likewise */
void jump_handlers_clear(void);
int jump_handlers_add(generic_fp entry);
void jump_handlers_apply(void);

/*
 * This routine jumps to an absolute address, regardless of MMU and page index
 * state.
 * by jumping to some address, callers acknowledge that C runtime components
 * like stack are not preserved, and no return-like operation will be performed.
 */
__noreturn
void abs_jump(void *addr);

/* get_addr_space()
 * determine whether we are running in low address or in high address
 * return values:
 * 0 - low address
 * 1 - high address
 * negative - reserved for errors
 *
 * FIXME:
 * Need a better name.  Also, hardwiring return values with
 * literals is probably not a good idea.
 * Can be changed into at_lower() or before_kernmap() or alike.
 */
int get_addr_space(void);

struct mm;

struct vma {
	addr_t		start;
	addr_t		end;
	unsigned long	flags;
	/* These flags match ELF segment flags */
#define VMA_EXEC	0x01
#define VMA_WRITE	0x02
#define VMA_READ	0x04
	struct mm	*mm;
	struct list_head node;
};

struct mm {
	struct list_head node;
	size_t		mmap_count;
	size_t		ref_count;
	pgindex_t	pgindex;
};

/* Map virtual address starting at @vaddr to page block @p */
int map_pages(struct mm *mm, void *vaddr, struct pages *p, uint32_t flags);
/* Unmap @nr_pages pages starting from virtual address @vaddr */
int unmap_pages(struct mm *mm, void *vaddr, size_t nr_pages);
/* Copy from kernel address @kvaddr to user space at @uvaddr */
int copy_to_uvm(struct mm *mm, void *uvaddr, void *kvaddr, size_t len);
/* Does the reverse */
int copy_from_uvm(struct mm *mm, void *uvaddr, void *kvaddr, size_t len);
/* Create a size @len user space mapping starting at virtual address @addr */
int create_uvm(struct mm *mm, void *addr, size_t len, uint32_t flags);
/* Destroy an entire user space mapping starting at @addr */
int destroy_uvm(struct mm *mm, void *addr);

#endif /* !__ASSEMBLER__ */

#endif /* _MM_H */

