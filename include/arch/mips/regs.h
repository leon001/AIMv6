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

#ifndef _ASM_REGS_H
#define _ASM_REGS_H

struct regs {
	/* general purpose registers */
	unsigned long	at;
	unsigned long	v0;
	unsigned long	v1;
	unsigned long	a0;
	unsigned long	a1;
	unsigned long	a2;
	unsigned long	a3;
	unsigned long	t0;
	unsigned long	t1;
	unsigned long	t2;
	unsigned long	t3;
	unsigned long	t4;
	unsigned long	t5;
	unsigned long	t6;
	unsigned long	t7;
	unsigned long	t8;
	unsigned long	t9;
	unsigned long	s0;
	unsigned long	s1;
	unsigned long	s2;
	unsigned long	s3;
	unsigned long	s4;
	unsigned long	s5;
	unsigned long	s6;
	unsigned long	s7;
	unsigned long	s8;
	unsigned long	gp;
	unsigned long	sp;
	unsigned long	ra;

	/* coprocessor registers */
	unsigned long	lo;
	unsigned long	hi;
	unsigned long	status;
	unsigned long	cause;
	unsigned long	epc;
	unsigned long	badvaddr;
};

#endif
