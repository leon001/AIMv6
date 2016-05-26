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

#ifndef _LIBC_SYSCALLS_H
#define _LIBC_SYSCALLS_H

/*
 * NOTES for system calls:
 * We plan to make these system calls POSIX-compliant, but this doesn't mean
 * that we will fully implement the behaviors specified by POSIX _now_.
 *
 * We will indicate to what extent each system call has been implemented.
 */

#define NR_SYSCALLS	20	/* number of system calls */

/*
 * int fork(void)
 */
#define NRSYS_fork	1

/*
 * int execve(int argc, const char *argv[], const char *envp[])
 * Environment is currently not implemented.
 */
#define NRSYS_execve	2
/*
 * pid_t waitpid(pid_t pid, int *status, int options)
 * We only implement "options == 0" case.
 * We only provide meaningful WIFEXITED() and WEXITSTATUS().
 */
#define NRSYS_waitpid	3
/*
 * int kill(pid_t pid, int sig)
 * Signals and signal handlers are not implemented.
 */
#define NRSYS_kill	4
/*
 * int sched_yield(void)
 */
#define NRSYS_sched_yield	5
/*
 * pid_t getpid(void)
 */
#define NRSYS_getpid	6
/*
 * int nanosleep(const struct timespec *req, struct timespec *rem)
 */
#define NRSYS_nanosleep	7
/*
 * pid_t getppid(void)
 */
#define NRSYS_getppid	8
/*
 * void exit(int status)
 */
#define NRSYS_exit	9

#endif
