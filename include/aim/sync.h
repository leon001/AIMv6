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

#ifndef _AIM_SYNC_H
#define _AIM_SYNC_H

#include <sys/types.h>

#include <arch-sync.h>	/* lock_t */
#include <irq.h>	/* local_irq_XXX */

#ifndef __ASSEMBLER__

/* Spinlocks. Implemented by architectures. */

/* By initializing a lock, caller assumes no code is holding it. */
void spinlock_init(lock_t *lock);
void spin_lock(lock_t *lock);
/* spin_unlock may contain instructions to send event */
void spin_unlock(lock_t *lock);
bool spin_is_locked(lock_t *lock);

#define spin_lock_irq_save(lock, flags) \
	do { \
		local_irq_save(flags); \
		spin_lock(lock); \
	} while (0)
#define spin_unlock_irq_restore(lock, flags) \
	do { \
		spin_unlock(lock); \
		local_irq_restore(flags); \
	} while (0)

/* Semaphore, implemented by architectures. */
typedef struct {
	int val;
	int limit;
} semaphore_t;

void semaphore_init(semaphore_t *sem, int val);
void semaphore_dec(semaphore_t *sem);
/* semaphore_inc may contain instructions to send event */
void semaphore_inc(semaphore_t *sem);
#define semaphore_pass(sem) ({ \
	semaphore_t *_sem = sem; \
	semaphore_dec(_sem); \
	semaphore_inc(_sem); })

#endif /* !__ASSEMBLER__ */

#endif /* _AIM_SYNC_H */

