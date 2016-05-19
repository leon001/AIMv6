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

#ifndef _SCHED_H
#define _SCHED_H

#include <proc.h>
#include <namespace.h>
#include <sys/types.h>

/*
 * struct proclist is implemented in scheduler source.
 *
 * Usually, different schedulers require different data structures for
 * process list.
 */
struct proclist;

struct scheduler {
	/* The actual data structure storing the set of proc */
	struct proclist	*proclist;
	/* Initialize scheduler and process list inside */
	void		(*init)(void);
	/* Remove and return an arbitrary proc from proc list */
	struct proc *	(*pick)(void);
	int		(*add)(struct proc *);
	/* Remove a specific proc from proc list */
	int		(*remove)(struct proc *);
	/* Retrieve next proc of @proc, NULL for first proc */
	struct proc *	(*next)(struct proc *);
	/*
	 * Locate the proc with PID @id and namespace @ns.
	 * Currently @ns should be always NULL.
	 */
	struct proc *	(*find)(pid_t pid, struct namespace *ns);
};

#endif
