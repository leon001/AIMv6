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

#ifndef _PMM_H
#define _PMM_H

#include <sys/types.h>
#include <vmm.h>

#ifndef __ASSEMBLER__

/* FIXME change name and create seperate header */
typedef uint32_t gfp_t;
/* currently ignored */

struct pages {
	addr_t paddr;
	addr_t size;
	gfp_t flags;
	atomic_t refs;	/* for shared memory */
	/*
	 * When swapping pages in and out, we normally save the page content
	 * and virtual page number (along with an identification of page
	 * index - could be PID if a page directory is _uniquely_ related
	 * to a process) together.  But if we are allowing shared memory,
	 * we need to save a _list_ of virtual page numbers (as well as page
	 * index identifications) since two virtual pages from different page
	 * indexes can be mapped to a same physical page, and if we are
	 * swapping such page out, we need to invalidate both virtual page
	 * entries.  To enable us finding all the virtual pages easily, we
	 * need to keep track of the list of virtual pages mapped to
	 * the same physical page (or page block).
	 *
	 * Here we are saving the list of such virtual pages inside the
	 * pages struct.  They are a list of struct vma (mm.h).
	 *
	 * FIXME: please review.  I think we need to wrap another layer
	 * outside `struct pages`, probably `struct mm_pages` or something,
	 * because the page allocator does not really care how many references
	 * are there and how many VMAs are mapped to a page.
	 */
	struct list_head share_vma_node;
};

struct page_allocator {
	int (*alloc)(struct pages *pages);
	void (*free)(struct pages *pages);
	addr_t (*get_free)(void);
};

int page_allocator_init(void);
int page_allocator_move(struct simple_allocator *old);
void set_page_allocator(struct page_allocator *allocator);
/* The registration above COPIES the struct. */

/* 
 * This interface may look wierd, but it prevents the page allocator from doing
 * any kmalloc-like allocation: it either breaks a block or remove a block upon
 * page allocation.
 * Returns 0 for success and EOF for failure.
 */
int alloc_pages(struct pages *pages);
void free_pages(struct pages *pages);
addr_t get_free_memory(void);

/* Returns -1 on error */
static inline addr_t pgalloc(void)
{
	struct pages p;
	p.size = PAGE_SIZE;
	p.flags = 0;
	if (alloc_pages(&p) != 0)
		return -1;
	return p.paddr;
}

static inline void pgfree(addr_t paddr)
{
	struct pages p;
	p.paddr = paddr;
	p.size = PAGE_SIZE;
	p.flags = 0;
	free_pages(&p);
}


/* initialize the page-block structure for remaining free memory */
void add_memory_pages(void);

#endif /* !__ASSEMBLER__ */

#endif /* _PMM_H */

