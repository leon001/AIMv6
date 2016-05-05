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

#ifndef _PGTABLE_H
#define _PGTABLE_H

#include <sys/types.h>

#define PTE_LOWMASK		0xfff
/* These bits match the mode bits in TLB entries */
#define PTE_CACHE_MASK		0xe00
# define PTE_CACHEABLE		0x600
# define PTE_UNCACHED		0x400
#define PTE_DIRTY		0x100
#define PTE_VALID		0x080
#define PTE_GLOBAL		0x040
/* Loongson 2F has this bit for buffer overflow protection.  Not sure whether
 * Loongson 3A has it. */
#define PTE_NOEXEC		0x008
/* Extra page table entry flags not needed by hardware */
#define PTE_SOFT_SHIFT		6
#define PTE_PHYS		0x001	/* physical address marker */

#define PTE_PADDR(pte)		((pte) & ~PTE_LOWMASK)
#define PTE_FLAGS(pte)		((pte) & PTE_LOWMASK)

#ifndef __LP64__	/* 32 bit */

typedef uint32_t pte_t, pde_t;

typedef pde_t pgindex_t;

#define PTXSHIFT	PAGE_SHIFT
#define PDXSHIFT	(PTXSHIFT + PAGE_SHIFT - WORD_SHIFT)

#define PDX(va)		(((ULCAST(va)) >> PDXSHIFT) & PTXMASK)
#define PTX(va)		(((ULCAST(va)) >> PTXSHIFT) & PTXMASK)

#else	/* 64 bit */

/*
 * We are assuming 48-bit physical address space, which is usually the case
 * (e.g. Loongson, AMD64)
 */

typedef uint64_t pte_t, pmd_t, pud_t, pgd_t;

typedef pgd_t pgindex_t;

#define PTXSHIFT	PAGE_SHIFT
#define PMXSHIFT	(PTXSHIFT + PAGE_SHIFT - WORD_SHIFT)
#define PUXSHIFT	(PMXSHIFT + PAGE_SHIFT - WORD_SHIFT)
#define PGXSHIFT	(PUXSHIFT + PAGE_SHIFT - WORD_SHIFT)

#define PGX(va)		(((ULCAST(va)) >> PGXSHIFT) & PTXMASK)
#define PUX(va)		(((ULCAST(va)) >> PUXSHIFT) & PTXMASK)
#define PMX(va)		(((ULCAST(va)) >> PMXSHIFT) & PTXMASK)
#define PTX(va)		(((ULCAST(va)) >> PTXSHIFT) & PTXMASK)

#endif	/* !__LP64__ */

#endif

