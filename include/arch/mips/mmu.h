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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* premap_addr() and postmap_addr() in addrspace.h */
#include <addrspace.h>
#include <util.h>

#ifdef PAGESIZE_16K
#define PAGE_SHIFT	14
#else
#define PAGE_SHIFT	12
#endif

#define PAGE_SIZE	(1 << PAGE_SHIFT)
#define PAGE_MASK	(PAGE_SIZE - 1)
#define PAGE_OFFSET(a)	(ULCAST(a) & PAGE_MASK)

#ifndef __ASSEMBLER__

#include <sys/types.h>
#include <pgtable.h>
#include <pmm.h>

void page_index_clear(pgindex_t *index);
int page_index_early_map(pgindex_t *index, addr_t paddr, size_t vaddr,
    size_t length);

#endif	/* !__ASSEMBLER__ */

#endif

