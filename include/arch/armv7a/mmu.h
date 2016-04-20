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

/*
 * This file contains MMU-related declarations, not to be confused with mm.h,
 * which contains general memory control routines.
 * This platform is currently named as ARMv7A, which means neither earlier
 * nor later ISA versions are supported. ARM has some kind of compatability
 * between versions, which we will make use in the future, and this platform
 * will be renamed. No interface should be changed anyway.
 */

#ifndef _ARCH_MMU_H
#define _ARCH_MMU_H

/* from kernel */
#include <sys/types.h>

#define ARM_SECT_SHIFT	20
#define ARM_SECT_SIZE	(1 << ARM_SECT_SHIFT)
#define ARM_PAGE_SHIFT	12
#define ARM_PAGE_SIZE	(1 << ARM_PAGE_SHIFT)

#define PAGE_SIZE	ARM_PAGE_SIZE

#define ARM_PT_AP_USER_NONE	0x1
#define ARM_PT_AP_USER_READ	0x2
#define ARM_PT_AP_USER_BOTH	0x3

#define ARM_PT_L1_RES		0x3
#define ARM_PT_L1_SECT		0x2
#define ARM_PT_L1_PAGE		0x1
#define ARM_PT_L1_FREE		0x0

/*
 * Existing naming solutions like pde_t and pte_t does not pass enough
 * information to developers, as there may be more than 2 levels of page
 * table hierarchy, and on some platforms the page mapping index may not even
 * come in the form of a page table.
 *
 * Declare MMU or platform-specific types with their names, and never use them
 * in interfaces.
 */

/*
 * ARM can utilize two seperate page tables at once, but enabling this feature
 * breaks the compatability with other platforms (who only use one).
 * Force no usage of TTBR1, and page table length will be fixed.
 */
#define ARM_PT_L1_LENGTH	4096
#define ARM_PT_L2_LENGTH	256

#ifndef __ASSEMBLER__

/*
 * ARM's page table entries have multiple formats for different usage, thus
 * defining structs are not encouraged.
 */
typedef uint32_t arm_pte_l1_t;
typedef uint32_t arm_pte_l2_t;

/*
 * Define page_index_head_t so interface routines can pass its pointer.
 */

typedef arm_pte_l1_t page_index_head_t;

void page_index_clear(page_index_head_t * index);
int page_index_early_map(page_index_head_t * index, addr_t paddr, size_t vaddr,
	size_t length);

#endif

#endif /* _ARCH_MMU_H */

