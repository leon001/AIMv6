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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <mmu.h>

#ifdef PAGESIZE_16K
#define PGTABLE_LEVEL	3
#else
#define PGTABLE_LEVEL	4
#endif

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

#define PTE_PADDR(pte)		((pte) & ~PTE_LOWMASK)
#define PTE_FLAGS(pte)		((pte) & PTE_LOWMASK)

#define PTXMASK		((1 << (PAGE_SHIFT - WORD_SHIFT)) - 1)
#define NR_PTENTRIES	(1 << (PAGE_SHIFT - WORD_SHIFT))

#ifndef __ASSEMBLER__
#include <sys/types.h>
#endif	/* !__ASSEMBLER__ */

#ifndef __LP64__	/* 32 bit */

#ifndef __ASSEMBLER__
typedef uint32_t pte_t, pde_t;

typedef uint32_t pgindex_t;
#endif	/* !__ASSEMBLER__ */

#define PTXSHIFT	PAGE_SHIFT
#define PDXSHIFT	(PTXSHIFT + PAGE_SHIFT - WORD_SHIFT)

#define PDX(va)		(((ULCAST(va)) >> PDXSHIFT) & PTXMASK)
#define PTX(va)		(((ULCAST(va)) >> PTXSHIFT) & PTXMASK)

#else	/* 64 bit */

/*
 * We are assuming 48-bit physical address space, which is usually the case
 * (e.g. Loongson, AMD64)
 */

#ifndef __ASSEMBLER__

#if PGTABLE_LEVEL == 3
typedef uint64_t pte_t, pmd_t, pgd_t;
#else	/* 4-level */
typedef uint64_t pte_t, pmd_t, pud_t, pgd_t;
#endif	/* PGTABLE_LEVEL */

typedef uint64_t pgindex_t;
#endif	/* !__ASSEMBLER__ */

#if PGTABLE_LEVEL == 3

#define PTXSHIFT	PAGE_SHIFT
#define PMXSHIFT	(PTXSHIFT + PAGE_SHIFT - WORD_SHIFT)
#define PGXSHIFT	(PMXSHIFT + PAGE_SHIFT - WORD_SHIFT)

#else	/* 4-level */

#define PTXSHIFT	PAGE_SHIFT
#define PMXSHIFT	(PTXSHIFT + PAGE_SHIFT - WORD_SHIFT)
#define PUXSHIFT	(PMXSHIFT + PAGE_SHIFT - WORD_SHIFT)
#define PGXSHIFT	(PUXSHIFT + PAGE_SHIFT - WORD_SHIFT)

#endif	/* PGTABLE_LEVEL */

#define PGX(va)		(((ULCAST(va)) >> PGXSHIFT) & PTXMASK)
#define PUX(va)		(((ULCAST(va)) >> PUXSHIFT) & PTXMASK)
#define PMX(va)		(((ULCAST(va)) >> PMXSHIFT) & PTXMASK)
#define PTX(va)		(((ULCAST(va)) >> PTXSHIFT) & PTXMASK)

#endif	/* !__LP64__ */

#ifndef __ASSEMBLER__
extern pgindex_t *pgdir_slots[];	/* Defined in vmaim.lds.S */
#endif	/* !__ASSEMBLER__ */

#endif

