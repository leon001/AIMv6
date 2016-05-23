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

#ifndef _ASM_STACK_H
#define _ASM_STACK_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <asm.h>
#include <smp.h>
#include <util.h>

#ifdef __ASSEMBLER__
	/* Assumes a correct kernel gp */
	.macro	get_saved_sp	reg temp
	cpuid	\temp, \reg
	SLL	\temp, WORD_SHIFT
	LA	\reg, kernelsp
	ADDU	\reg, \temp
	LOAD	\reg, (\reg)	/* kernelsp + CPUID << WORD_SHIFT */
	.endm

	/* Assumes a correct kernel gp */
	.macro	set_saved_sp	stackp temp temp2
	cpuid	\temp, \temp2
	SLL	\temp, WORD_SHIFT
	LA	\temp2, kernelsp
	ADDU	\temp2, \temp
	STORE	\stackp, (\temp2)
	.endm

	/* Assumes a working sp */
	.macro	PUSH	reg
	SUBU	sp, WORD_SIZE
	STORE	\reg, (sp)
	.endm

	/* Assumes a working sp */
	.macro	POP	reg
	LOAD	\reg, (sp)
	ADDU	sp, WORD_SIZE
	.endm

	.macro	PUSHSTATIC
	PUSH	s7
	PUSH	s6
	PUSH	s5
	PUSH	s4
	PUSH	s3
	PUSH	s2
	PUSH	s1
	PUSH	s0
	.endm

	.macro	POPSTATIC
	POP	s0
	POP	s1
	POP	s2
	POP	s3
	POP	s4
	POP	s5
	POP	s6
	POP	s7
	.endm

	.macro	PUSHTEMP
	PUSH	t7
	PUSH	t6
	PUSH	t5
	PUSH	t4
	PUSH	t3
	PUSH	t2
	PUSH	t1
	PUSH	t0
	.endm

	.macro	POPTEMP
	POP	t0
	POP	t1
	POP	t2
	POP	t3
	POP	t4
	POP	t5
	POP	t6
	POP	t7
	.endm

	.macro	PUSHARGS
	PUSH	a3
	PUSH	a2
	PUSH	a1
	PUSH	a0
	PUSH	v1
	PUSH	v0
	PUSH	AT
	PUSH	zero	/* Theoretically we don't need it */
	.endm

	.macro	POPARGS
	POP	AT	/* Discard the stored zero */
	POP	AT
	POP	v0
	POP	v1
	POP	a0
	POP	a1
	POP	a2
	POP	a3
	.endm

	.macro	PUSHAL
	PUSHSTATIC
	PUSHTEMP
	PUSHARGS
	.endm

	.macro	POPAL
	POPARGS
	POPTEMP
	POPSTATIC
	.endm

	.macro	PUSHCOPR reg
	MFC0	\reg, CP0_BADVADDR
	PUSH	\reg
	MFC0	\reg, CP0_EPC
	PUSH	\reg
	mfc0	\reg, CP0_CAUSE
	PUSH	\reg
	mfc0	\reg, CP0_STATUS
	PUSH	\reg
	mfhi	\reg
	PUSH	\reg
	mflo	\reg
	PUSH	\reg
	.endm

	.macro	POPCOPR reg
	POP	\reg
	mtlo	\reg
	POP	\reg
	mthi	\reg
	POP	\reg
	mtc0	\reg, CP0_STATUS
	POP	\reg
	mtc0	\reg, CP0_CAUSE
	POP	\reg
	MTC0	\reg, CP0_EPC
	POP	\reg
	MTC0	\reg, CP0_BADVADDR	/* discarded */
	.endm

	.macro	PUSHUP	reg base
	ADDU	\base, WORD_SIZE
	STORE	\reg, (\base)
	.endm

	.macro	PUSHDOWN reg base
	SUBU	\base, WORD_SIZE
	STORE	\reg, (\base)
	.endm

	.macro	POPUP	reg base
	LOAD	\reg, (\base)
	ADDU	\base, WORD_SIZE
	.endm

	.macro	POPDOWN	reg base
	LOAD	\reg, (\base)
	SUBU	\base, WORD_SIZE
	.endm

#else	/* !__ASSEMBLER__ */
#include <sys/types.h>
extern unsigned long kernelsp[NR_CPUS];
#endif	/* __ASSEMBLER__ */

#endif
