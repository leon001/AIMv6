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

#include <memlayout.h>
#include <util.h>

/* addresses before and after early MMU mapping */
#define __premap_addr(kva)	(ADDR_CAST(kva) - KERN_BASE)
#define __postmap_addr(pa)	(ADDR_CAST(pa) + KERN_BASE)

/* kernel virtual address and physical address conversion */
#define kva2pa(kva)		(ADDR_CAST(kva) - KERN_BASE)
#define pa2kva(pa)		(ADDR_CAST(pa) + KERN_BASE)

#define PAGE_SHIFT	12
#define PTX_SHIFT	PAGE_SHIFT
#define PDX_SHIFT	(PTX_SHIFT + PAGE_SHIFT - 2)
#define PAGE_SIZE	(1 << PAGE_SHIFT)
#define XPAGE_SHIFT	22
#define XPTX_SHIFT	XPAGE_SHIFT
#define XPAGE_SIZE	(1 << XPAGE_SHIFT)	/* 4MB */

/* Page table flags */
#define PTE_P		0x1		/* Present */
#define PTE_R		0x2		/* Read-write */
#define PTE_U		0x4		/* User */
#define PTE_W		0x8		/* Write-through */
#define PTE_C		0x10		/* Disable cache */
#define PTE_A		0x20		/* Accessed */
#define PTE_D		0x40		/* Dirty */
#define PTE_S		0x80		/* Big pages */
#define PTE_G		0x100		/* Global */
#define PTE_X1		0x200		/* Unused */
#define PTE_X2		0x400		/* Unused */
#define PTE_X3		0x800		/* Unused */
/* More readable alternative names */
#define PTE_PRESENT	PTE_P
#define PTE_WRITABLE	PTE_R
#define PTE_USER	PTE_U
#define PTE_WRITETHRU	PTE_W
#define PTE_NOCACHE	PTE_C
#define PTE_ACCESSED	PTE_A
#define PTE_DIRTY	PTE_D
#define PTE_BIGPAGE	PTE_S

#define mkxpte(paddr, flags) \
	(ALIGN_BELOW(paddr, XPAGE_SIZE) | (flags))
#define mkpde(paddr, flags) \
	(ALIGN_BELOW(paddr, PAGE_SIZE) | (flags))
#define mkpte(paddr, flags) \
	(ALIGN_BELOW(paddr, PAGE_SIZE) | (flags))

#define PTX(vaddr)	((vaddr) >> PTX_SHIFT)
#define PDX(vaddr)	((vaddr) >> PDX_SHIFT)
#define XPTX(vaddr)	((vaddr) >> XPTX_SHIFT)

#define PTE_LOWMASK		0xfff

#define PTE_PADDR(pte)		((pte) & ~PTE_LOWMASK)
#define PTE_FLAGS(pte)		((pte) & PTE_LOWMASK)

#ifndef __ASSEMBLER__

#include <sys/types.h>

typedef uint32_t pte_t, pde_t, xpte_t;

/* physical address of page table */
typedef uint32_t pgindex_t;

void page_index_clear(pgindex_t * index);
/*
 * returns -1 if @paddr and @vaddr are not aligned
 */
int page_index_early_map(pgindex_t * index, addr_t paddr, size_t vaddr,
	size_t length);
#endif

#endif
