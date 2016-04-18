/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
 * Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
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

#define premap_addr(kva)	((kva) - KERN_BASE)
#define postmap_addr(pa)	((pa) + KERN_BASE)
#define PAGE_SHIFT	12
#define PAGE_SIZE	(1 << PAGE_SHIFT)

#ifndef __ASSEMBLER__
#include <sys/types.h>
typedef uint32_t pte_t, pde_t;

typedef pde_t page_index_head_t;

void page_index_clear(page_index_head_t * index);
int page_index_early_map(page_index_head_t * index, addr_t paddr, size_t vaddr,
	size_t length);
#endif

#endif
