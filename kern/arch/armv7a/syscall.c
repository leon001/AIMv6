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

#include <syscall.h>
#include <arch-trap.h>
#include <mm.h>
#include <percpu.h>
#include <errno.h>

int syscall_no(struct trapframe *tf)
{
	return tf->r7;
}

int syscall_arg(struct trapframe *tf, int index, unsigned long *result)
{
	unsigned long addr;
	if (index < 7) {
		*result = (&tf->r0)[index];
	} else {
		addr = tf->sp + index * WORD_SIZE;

		/* We should defend for corrupted SP value here. */
		if (!is_user(addr) && !from_kernel(tf))
			/* TODO: should signal the process to terminate */
			return -EFAULT;
		*result = *(unsigned long *)addr;
	}
	return 0;
}

void syscall_return(struct trapframe *tf, unsigned long long result)
{
	/* safe for both 32bit and 64bit return values.s */
	tf->r0 = (unsigned long)result;
	tf->r1 = (unsigned long)(result >> 32);
	/*
	 * ARM does not have error boolean register, which is true
	 * both in EABI and in OABI.
	 */
}

