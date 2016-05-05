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
	int ret = 1;
	/* hard */
	while (ret != 0) {
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
	SMP_DMB();
	*lock = UNLOCKED;
	SMP_DSB();
	asm volatile ("sev");
}

