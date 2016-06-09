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

#ifndef _ASM_SMP_H
#define _ASM_SMP_H

#include <asm.h>
#include <regdef.h>
#include <cp0regdef.h>

/*
 * Implementation of cpuid() macro and cpuid assembly macro to get the
 * current core ID#.
 */
#if __mips_isa_rev == 1	/* Rev. 1 */
/*
 * Since MIPS32/64 Revision 1 don't provide EBASE register, how to retrieve
 * the core ID# is machine-dependent.
 * Provide __cpuid() function in mach/smp.h
 */
#include <mach-smp.h>
#else	/* Rev. 2 or higher */
/*
 * Revision 2 or higher provides the EBASE register, where the core ID#
 * is encoded.
 */
#ifndef __ASSEMBLER__
#include <mipsregs.h>
/*
 * The header is included by a C header/source.
 */
static inline unsigned int __cpuid(void)
{
	return read_c0_ebase() & EBASE_CPUNUM_MASK;
}

#define cpuid()		__cpuid()

#else	/* __ASSEMBLER__ */
/*
 * The header is included by an assembly header/source.
 */
	.macro	cpuid result temp
	MFC0	\result, CP0_EBASE
	andi	\result, EBASE_CPUNUM_MASK
	.endm
#endif	/* !__ASSEMBLER__ */

#endif	/* Rev. */

#ifndef __ASSEMBLER__
#include <mmu.h>
#include <stack.h>
#define current_kernelsp	kernelsp[cpuid()]
#define current_pgdir		pgdir_slots[cpuid()]
#endif	/* !__ASSEMBLER__ */

#endif
