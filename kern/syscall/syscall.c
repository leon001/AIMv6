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
#include <libc/syscalls.h>
#include <trap.h>
#include <errno.h>
#include <libc/stddef.h>

static syscall_t __syscalls[NR_SYSCALLS] = {0};

static void __syscall_nonexist(struct trapframe *tf)
{
	syscall_set_errno(tf, ENOSYS);
	syscall_return(tf, -1);
}

static void __syscall_fail(struct trapframe *tf, int error)
{
	syscall_set_errno(tf, error);
	syscall_return(tf, -1);
}

void handle_syscall(struct trapframe *tf)
{
	int i, retcode, errno;
	int sysno = syscall_no(tf);
	/* We assume that there are no system calls with more than 8
	 * arguments. */
	unsigned long args[8];
	unsigned long long result;

	if (__syscalls[sysno] == NULL) {
		__syscall_nonexist(tf);
	} else {
		for (i = 0; i < 8; ++i) {
			retcode = syscall_arg(tf, i, &args[i]);
			if (retcode < 0) {
				__syscall_fail(tf, -retcode);
				return;
			}
		}
		/*
		 * Here comes the magic.
		 * We made all kernel system call handler entries to be
		 * variadic (see definition of syscall_t).  So here
		 * we are essentially leaving the argument resolutions
		 * specified by ABI to compiler.
		 *
		 * The kernel is not dealing with multiple ABIs on the
		 * same platform.
		 */
		result = __syscalls[sysno](&errno, args[0], args[1],
					   args[2], args[3], args[4],
					   args[5], args[6], args[7]);
		syscall_return(tf, result);
		syscall_set_errno(tf, errno);
	}
}

void register_syscall(syscall_t syscall, int sysno)
{
	/* TODO: deal with overlapping system call numbers */
	__syscalls[sysno] = syscall;
}

