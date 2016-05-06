/* Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
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

#ifndef _MMU_H
#define _MMU_H

/* premap_addr() and postmap_addr() in addrspace.h */
#include <addrspace.h>
#include <asm.h>	/* WORD_SHIFT */
#include <util.h>

#define PAGE_SHIFT	12
#define PAGE_SIZE	(1 << PAGE_SHIFT)
#define PAGE_MASK	(PAGE_SIZE - 1)

#define PTXMASK		((1 << (PAGE_SHIFT - WORD_SHIFT)) - 1)
#define NR_PTENTRIES	(1 << (PAGE_SHIFT - WORD_SHIFT))

#ifndef __ASSEMBLER__

#include <sys/types.h>
#include <pgtable.h>

void page_index_clear(pgindex_t *index);
int page_index_early_map(pgindex_t *index, addr_t paddr, size_t vaddr,
    size_t length);

/*
 * Fixed-size single page allocation is not applicable on all platforms.
 */

/* Returns -1 on error */
static inline addr_t pgalloc(void)
{
	struct pages p = {0, PAGE_SIZE, 0};
	if (alloc_pages(&p) != 0)
		return -1;
	return p.paddr;
}

static inline void pgfree(addr_t paddr)
{
	struct pages p = {paddr, PAGE_SIZE, 0};
	free_pages(&p);
}

#endif	/* !__ASSEMBLER__ */

#endif

