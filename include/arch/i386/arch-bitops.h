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

#ifndef _ARCH_BITOPS_H
#define _ARCH_BITOPS_H

#define ffs(x)	__ffs(x)
#define fls(x)	__fls(x)
#define ffz(x)	__ffz(x)
#define flz(x)	__flz(x)

static inline int __ffs(unsigned long x)
{
	int r;
	asm (
		"	bsfl	%1, %0;"
		"	jnz	1f;"
		"	movl	$-1, %0;"
		"1:"
		: "=r"(r)
		: "rm"(x)
	);
	return r + 1;
}

static inline int __fls(unsigned long x)
{
	int r;
	asm (
		"	bsrl	%1, %0;"
		"	jnz	1f;"
		"	movl	$-1, %0;"
		"1:"
		: "=r"(r)
		: "rm"(x)
	);
	return r + 1;
}

static inline int __ffz(unsigned long x)
{
	return __ffs(~x);
}

static inline int __flz(unsigned long x)
{
	return __fls(~x);
}

#endif
