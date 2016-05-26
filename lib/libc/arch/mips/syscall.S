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

#include <asm.h>
#include <regdef.h>
#include <libc/syscalls.h>

/*
 * The arguments are already organized by C compiler as per MIPS ABI, so
 * we only need to load the system call number, trigger an exception,
 * and jump back.
 *
 * As per Linux system call convention, we put system call number in
 * register v0.
 *
 * Register a3 holds error code after system call handling (not used
 * currently).
 */

/*
 * The system call convention on MIPS, used by Linux and BSD, goes as follows:
 * 1. v0 stores the system call number before system call.
 * 2. a3 stores the error code after system call.
 * 3. Everything else follows the standard ABI calling convention.
 */

#define DEFINE_SYSCALL(name) \
NESTED(name, 32, ra); \
	.set	noreorder; \
	.cpload t9; \
	.set	reorder; \
	SUBU	sp, 32; \
	.cprestore 16; \
	li	v0, NRSYS_##name; \
	syscall; \
	beqz	a3, 1f; \
	move	a0, a3; \
	jal	__set_errno; \
1:	ADDU	sp, 32; \
	jr	ra; \
	nop; \
END(name);

DEFINE_SYSCALL(fork)
DEFINE_SYSCALL(execve)
DEFINE_SYSCALL(waitpid)
DEFINE_SYSCALL(kill)
DEFINE_SYSCALL(sched_yield)
DEFINE_SYSCALL(getpid)
DEFINE_SYSCALL(nanosleep)
DEFINE_SYSCALL(getppid)
DEFINE_SYSCALL(exit)

