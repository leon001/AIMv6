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

#ifndef _ATOMIC_H
#define _ATOMIC_H

#include <sys/types.h>
#include <arch-sync.h>

#ifndef __ASSEMBLER__

/* counter += val */
static inline void atomic_add(atomic_t *counter, uint32_t val)
{
	register int reg;
	int ret = ARM_STREX_FAIL;

	while (ret == ARM_STREX_FAIL) {
		SMP_DMB();
		asm volatile (
			"ldrex	%[reg], [%[addr]];"
			"add	%[reg], %[reg], %[val];"
			"strex	%[ret], %[reg], [%[addr]];"
			/* STREX happens only at signed less or equal. */
			: [reg] "=r" (reg),
			  [ret] "=r" (ret)
			: [val] "i" (val),
			  [addr] "r" (counter)
		);
	}
	SMP_DMB();
}

/* counter -= val */
static inline void atomic_sub(atomic_t *counter, uint32_t val)
{
	register int reg;
	int ret = ARM_STREX_FAIL;

	while (ret == ARM_STREX_FAIL) {
		SMP_DMB();
		asm volatile (
			"ldrex	%[reg], [%[addr]];"
			"sub	%[reg], %[reg], %[val];"
			"strex	%[ret], %[reg], [%[addr]];"
			/* STREX happens only at signed less or equal. */
			: [reg] "=r" (reg),
			  [ret] "=r" (ret)
			: [val] "i" (val),
			  [addr] "r" (counter)
		);
	}
	SMP_DMB();
}

static inline void atomic_set_bit(unsigned long nr,
				  volatile unsigned long *addr)
{
	unsigned long *m = ((unsigned long *)addr) + ((--nr) >> BITS_PER_LONG_LOG);
	int bit = nr & BITS_PER_LONG_MASK;
	int ret = ARM_STREX_FAIL;
	register int reg;

	while (ret == ARM_STREX_FAIL) {
		SMP_DMB();
		asm volatile (
			"ldrex	%[reg], [%[addr]];"
			"orr	%[reg], %[reg], %[val];"
			"strex	%[ret], %[reg], [%[addr]];"
			/* STREX happens only at signed less or equal. */
			: [reg] "=r" (reg),
			  [ret] "=r" (ret)
			: [val] "r" (1UL << bit),
			  [addr] "r" (m)
		);
	}
	SMP_DMB();
}

static inline void atomic_clear_bit(unsigned long nr,
				  volatile unsigned long *addr)
{
	unsigned long *m = ((unsigned long *)addr) + ((--nr) >> BITS_PER_LONG_LOG);
	int bit = nr & BITS_PER_LONG_MASK;
	int ret = ARM_STREX_FAIL;
	register int reg;

	while (ret == ARM_STREX_FAIL) {
		SMP_DMB();
		asm volatile (
			"ldrex	%[reg], [%[addr]];"
			"bic	%[reg], %[reg], %[val];"
			"strex	%[ret], %[reg], [%[addr]];"
			/* STREX happens only at signed less or equal. */
			: [reg] "=r" (reg),
			  [ret] "=r" (ret)
			: [val] "r" (1UL << bit),
			  [addr] "r" (m)
		);
	}
	SMP_DMB();
}

#define atomic_inc(x)	atomic_add((x), 1)
#define atomic_dec(x)	atomic_sub((x), 1)

#endif /* !__ASSEMBLER__ */

#endif /* _ATOMIC_H */

