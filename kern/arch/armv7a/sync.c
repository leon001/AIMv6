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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

/* 
 * This file implements the synchronization between cores,
 * including a spinlock and a spin-wait signal.
 */

#include <aim/sync.h>
#include <panic.h>

/*
 * spinlock
 * Note that the lock option is written out explicitly in entry.S
 * The implementation there MUST work together with this one.
 */

void spinlock_init(lock_t *lock)
{
	/* since no one is holding this lock, we write it out directly */
	*lock = UNLOCKED;
	/* make it visible */
	SMP_DMB();
}

void spin_lock(lock_t *lock)
{
	register int val;
	int ret = ARM_STREX_FAIL;

	while (ret != ARM_STREX_SUCCESS) {
		asm volatile (
			"ldrex		%[val], [%[addr]];"
			"cmp		%[val], %[unlocked];"
			"wfene;"
			"strexeq	%[ret], %[locked], [%[addr]];"
			: [val] "=r" (val),
			  [ret] "=r" (ret)
			: [locked] "r" (LOCKED),
			  [unlocked] "i" (UNLOCKED),
			  [addr] "r" (lock)
		);
	}
	SMP_DMB();
}

void spin_unlock(lock_t *lock)
{
	/*
	 * if caller trys to unlock a lock that is not locked (by him),
	 * some data structure must be broken.
	 */
	if (*lock == UNLOCKED)
		panic("Unlocking not-owned lock at 0x%08x\n", lock);

	SMP_DMB();
	*lock = UNLOCKED;
	SMP_DSB();
	asm volatile ("sev");
}

/* Semaphore */

void semaphore_init(semaphore_t *sem, int val)
{
	/* we write it out directly */
	sem->val = val;
	sem->limit = val;
	/* make it visible */
	SMP_DMB();
}

void semaphore_dec(semaphore_t *sem)
{
	register int val;
	int ret = ARM_STREX_FAIL;

	while (ret != ARM_STREX_SUCCESS) {
		asm volatile (
			"ldrex		%[val], [%[addr]];"
			"subs		%[val], %[val], %[amount];"
			"wfemi;"/* minus */
			"strexpl	%[ret], %[val], [%[addr]];"
			/* pl means positive or zero */
			: [val] "=r" (val),
			  [ret] "=r" (ret)
			: [amount] "i" (1),
			  [addr] "r" (&sem->val)
		);
	}
	SMP_DMB();
}

void semaphore_inc(semaphore_t *sem)
{
	register int val;
	int ret = ARM_STREX_FAIL;

	/*
	 * Things are put inside the loop because LDREX gets a different
	 * value per try. ONE SINGLE OVERFLOW MEANS DATA CORRUPTION.
	 */
	while (ret == ARM_STREX_FAIL) {
		SMP_DMB();
		asm volatile (
			"ldrex		%[val], [%[addr]];"
			"add		%[val], %[val], %[amount];"
			"cmp		%[val], %[limit];"
			"movgt		%[ret], %[corrupt];"
			"strexle	%[ret], %[val], [%[addr]];"
			/* STREX happens only at signed less or equal. */
			: [val] "=r" (val),
			  [ret] "=r" (ret)
			: [amount] "i" (1),
			  [corrupt] "i" (ARM_STREX_CORRUPT),
			  [limit] "ir" (sem->limit),
			  [addr] "r" (&sem->val)
		);
	}
	if (ret == ARM_STREX_CORRUPT)
		panic("Increasing semaphore at 0x%08x to %d/%d\n", \
			sem, val, sem->limit);
	SMP_DSB();
}

