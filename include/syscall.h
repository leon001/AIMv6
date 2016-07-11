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

#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <aim/initcalls.h>
#include <trap.h>

/*
 * Because ISO C requires a named argument before ellipsis (...), all
 * system call handlers shall receive an additional argument containing
 * the system call number before its won arguments.
 *
 * E.g.
 *     fork(void) -> sys_fork(struct trapframe *tf, int *errno)
 *     exit(int code) -> sys_exit(struct trapframe *tf, int *errno, int code)
 */
typedef unsigned long long (*syscall_t)(struct trapframe *tf, int *errno, ...);

/*
 * Arch-independent
 */
void handle_syscall(struct trapframe *tf);
void register_syscall(syscall_t syscall, int sysno);

/*
 * Arch-specific internal functions to obtain system call number.
 */
int syscall_no(struct trapframe *tf);
void syscall_set_errno(struct trapframe *tf, int errno);
/*
 * Arch-specific function to get the (i+1)th argument of system call.
 */
int syscall_arg(struct trapframe *tf, int index, unsigned long *result);
/*
 * Arch-specific function to return a value and set error number
 */
void syscall_return(struct trapframe *tf, unsigned long long result, int errno);

#define ADD_SYSCALL(entry, sysno) \
	static int __##entry##_init(void) \
	{ \
		register_syscall((syscall_t)entry, sysno); \
		return 0; \
	} \
	INITCALL_SYSCALLS(__##entry##_init);

#endif
