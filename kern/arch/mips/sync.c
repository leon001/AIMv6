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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <aim/sync.h>
#include <panic.h>
#include <sys/types.h>

void spinlock_init(lock_t *lock)
{
	*lock = UNLOCKED;
	/* Currently as Loongson 3A automatically handles hazards and
	 * consistency, and MSIM is deterministic, we don't care about
	 * barriers here. */
}

void spin_lock(lock_t *lock)
{
	uint32_t reg;
	asm volatile (
		"1:	ll	%[reg], %[mem];"
		"	beqz	%[reg], 2f;"
		"	sc	%[reg], %[mem];"
		"	b	1b;"
		"2:	or	%[reg], 1;"
		"	sc	%[reg], %[mem];"
		"	beqz	%[reg], 1b;"
		: [reg]"=&r"(reg), [mem]"+m"(*lock)
	);
}

void spin_unlock(lock_t *lock)
{
	uint32_t reg;
	asm volatile (
		"1:	ll	%[reg], %[mem];"
		"	and	%[reg], ~1;"
		"	sc	%[reg], %[mem];"
		"	beqz	%[reg], 1b;"
		: [reg]"=&r"(reg), [mem]"+m"(*lock)
	);
}

