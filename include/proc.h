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

#ifndef _PROC_H
#define _PROC_H

#include <sys/types.h>
#include <namespace.h>

#define PROC_NAME_LEN_MAX	256

struct proc {
	/*
	 * the kernel stack pointer is used to prepare C runtime, thus accessed
	 * in assembly. Force it here at offset 0 for easy access.
	 */
	void *kstack; /* bottom, or lower address */
	size_t kstack_size;

	/* other stuff go here */
	int		tid;	/* Thread ID (unused - for multithreading) */
	int		pid;	/* Process ID within namespace @namespace */
	int		kpid;	/* Process ID */
	unsigned int	state;	/* Process state (runnability) */
	/* The state values come from OpenBSD */
	/* TODO: may have more...? */
#define PS_EMBRYO	1	/* Just created */
#define PS_RUNNABLE	2	/* Runnable */
#define PS_SLEEPING	3	/* Currently sleeping */
#define PS_ZOMBIE	4
#define	PS_ONPROC	5
	unsigned int	exit_code;	/* Exit code */
	unsigned int	exit_signal;	/* Signal code */
	uint32_t	flags;	/* Flags */
	/* TODO: may have more...? */
#define PF_EXITING	0x00000004	/* getting shut down */
#define PF_SIGNALED	0x00000400	/* killed by a signal */
	int		oncpu;		/* CPU ID being running on */
#define CPU_NONE	-1
	uintptr_t	bed;		/* address we are sleeping on */
	struct namespace *namespace;	/* Namespace */
	struct mm 	*mm; /* Memory mapping structure including pgindex */
	/*
	 * Expandable heap is placed directly above user stack.
	 * User stack is placed above all loaded program segments.
	 * We put program arguments above user stack.
	 */
	struct regs	*context;	/* Context before switch */
	size_t		heapsize;	/* Expandable heap size */

	/* TODO: do we need these? */
	uintptr_t	ustacktop;	/* User stack top */
	uintptr_t	progtop; /* Top of all segments (page-aligned) */

	char		name[PROC_NAME_LEN_MAX];

	/* Process tree related */
	struct proc	*parent;
	struct proc	*first_child;
	struct proc	*next_sibling;
	struct proc	*prev_sibling;
	struct list_head proc_node;
};

/* Create a struct proc inside namespace @ns and initialize everything if we
 * can by default. */
struct proc *proc_new(struct namespace *ns);

#endif /* _PROC_H */

