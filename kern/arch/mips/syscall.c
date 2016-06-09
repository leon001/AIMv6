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

#include <syscall.h>
#include <arch-trap.h>
#include <mipsregs.h>
#include <mm.h>
#include <percpu.h>
#include <errno.h>

/*
 * See lib/libc/arch/mips/syscall.S for how to retrieve system call number
 * and arguments, and how to set up error code and return value.
 */

int syscall_no(struct trapframe *tf)
{
	return tf->gpr[_V0];
}

#ifdef __LP64__	/* 64 bit */

int syscall_arg(struct trapframe *tf, int index, unsigned long *result)
{
	unsigned long addr;

	if (index < 8) {
		*result = tf->gpr[_A0 + index];
	} else {
		/*
		 * NOTREACHED.
		 * We only consider no more than 8 system call arguments.
		 */
		addr = tf->gpr[_SP] + (index - 8) * WORD_SIZE;

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
	tf->gpr[_V0] = result;
}

#else	/* 32 bit */

int syscall_arg(struct trapframe *tf, int index, unsigned long *result)
{
	unsigned long addr;
	if (index < 4) {
		*result = tf->gpr[_A0 + index];
	} else {
		addr = tf->gpr[_SP] + index * WORD_SIZE;

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
	tf->gpr[_V0] = (unsigned long)result;
	tf->gpr[_V1] = (unsigned long)(result >> 32);
}

#endif	/* __LP64__ */

void syscall_set_errno(struct trapframe *tf, int errno)
{
	tf->gpr[_A3] = errno;
}

