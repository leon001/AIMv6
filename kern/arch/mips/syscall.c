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

/*
 * See lib/libc/arch/mips/syscall.S for how to retrieve system call number
 * and arguments, and how to set up error code and return value.
 */

int syscall_no(struct trapframe *tf)
{
	return tf->gpr[_V0];
}

#ifdef __LP64__	/* 64 bit */

unsigned long syscall_arg(struct trapframe *tf, int index)
{
	unsigned long addr;

	if (index < 8) {
		return tf->gpr[_A0 + index];
	} else {
		addr = tf->gpr[_SP] + (index - 8) * WORD_SIZE;

		/* TODO: SHOULD BE REMOVED after implementing user space
		 * This only serves for quick tests for system calls. */
		if (!is_user(addr))
			return *(unsigned long *)addr;

		return *(unsigned long *)uva2kva(current_proc->mm->pgindex,
		    (void *)addr);
	}
}

void syscall_return(struct trapframe *tf, unsigned long long result)
{
	tf->gpr[_V0] = result;
}

#else	/* 32 bit */

unsigned long syscall_arg(struct trapframe *tf, int index)
{
	unsigned long addr;
	if (index < 4) {
		return tf->gpr[_A0 + index];
	} else {
		addr = tf->gpr[_SP] + index * WORD_SIZE;

		/* TODO: SHOULD BE REMOVED after implementing user space
		 * This only serves for quick tests for system calls. */
		if (!is_user(addr))
			return *(unsigned long *)addr;

		return *(unsigned long *)uva2kva(current_proc->mm->pgindex,
		    (void *)addr);
	}
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

