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

#define PAGE_SHIFT	12
#define PAGE_SIZE	(1 << PAGE_SHIFT)

#ifndef __ASSEMBLER__

#include <sys/types.h>

typedef uint32_t pte_t, pde_t;

/* TODO: I wonder if something like pgtable_t is better */
typedef pde_t pgindex_t;

void page_index_clear(pgindex_t *index);
int page_index_early_map(pgindex_t *index, addr_t paddr, size_t vaddr,
    size_t length);

#endif

#endif
