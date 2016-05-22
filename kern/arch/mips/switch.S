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

#include <asm.h>
#include <regdef.h>
#include <cp0regdef.h>
#include <stack.h>

/* switch_regs(struct regs *old, struct regs *new) */

/* This function merely switches registers and you can safely
 * ignore this implementation if you're not interested. */
LEAF(switch_regs)
	.set	push
	.set	noat
	PUSHUP	zero, a0
	PUSHUP	AT, a0
	PUSHUP	v0, a0
	PUSHUP	v1, a0
	PUSHUP	a0, a0
	PUSHUP	a1, a0
	PUSHUP	a2, a0
	PUSHUP	a3, a0
	PUSHUP	t0, a0
	PUSHUP	t1, a0
	PUSHUP	t2, a0
	PUSHUP	t3, a0
	PUSHUP	t4, a0
	PUSHUP	t5, a0
	PUSHUP	t6, a0
	PUSHUP	t7, a0
	PUSHUP	s0, a0
	PUSHUP	s1, a0
	PUSHUP	s2, a0
	PUSHUP	s3, a0
	PUSHUP	s4, a0
	PUSHUP	s5, a0
	PUSHUP	s6, a0
	PUSHUP	s7, a0
	PUSHUP	t8, a0
	PUSHUP	t9, a0
	PUSHUP	k0, a0
	PUSHUP	k1, a0
	PUSHUP	gp, a0
	PUSHUP	sp, a0
	PUSHUP	s8, a0
	PUSHUP	ra, a0
	mflo	a2
	PUSHUP	a2, a0
	mfhi	a2
	PUSHUP	a2, a0
	mfc0	a2, CP0_STATUS
	PUSHUP	a2, a0
	mfc0	a2, CP0_CAUSE
	PUSHUP	a2, a0
	MFC0	a2, CP0_EPC
	PUSHUP	a2, a0
	MFC0	a2, CP0_BADVADDR
	PUSHUP	a2, a0

	POPUP	a2, a1	/* discard zero */
	POPUP	AT, a1
	POPUP	v0, a1
	POPUP	v1, a1
	POPUP	a0, a1
	POPUP	a2, a1	/* discard a1 */
	POPUP	a2, a1
	POPUP	a3, a1
	POPUP	t0, a1
	POPUP	t1, a1
	POPUP	t2, a1
	POPUP	t3, a1
	POPUP	t4, a1
	POPUP	t5, a1
	POPUP	t6, a1
	POPUP	t7, a1
	POPUP	s0, a1
	POPUP	s1, a1
	POPUP	s2, a1
	POPUP	s3, a1
	POPUP	s4, a1
	POPUP	s5, a1
	POPUP	s6, a1
	POPUP	s7, a1
	POPUP	t8, a1
	POPUP	t9, a1
	POPUP	k0, a1
	POPUP	k1, a1
	POPUP	gp, a1
	POPUP	sp, a1
	POPUP	s8, a1
	POPUP	ra, a1
	POPUP	a2, a1
	mtlo	a2
	POPUP	a2, a1
	mthi	a2
	POPUP	a2, a1
	mtc0	a2, CP0_STATUS
	POPUP	a2, a1
	mtc0	a2, CP0_CAUSE
	POPUP	a2, a1
	MTC0	a2, CP0_EPC
	POPUP	a2, a1
	MTC0	a2, CP0_BADVADDR
	.set	pop
END(switch_regs)
