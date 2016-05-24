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

#include <proc.h>
#include <bitmap.h>
#include <namespace.h>
#include <aim/sync.h>

/*
 * Plain round-robin scheduler implementation.
 */

struct proclist {
	lock_t lock;
	struct list_head head;	/* head of process list */
};

struct plain_scheduler {
	struct scheduler;
	struct proclist proclist;
	struct proc *current;
};

static struct plain_scheduler plain_scheduler;

static struct proc *__sched_plain_pick(void)
{
	list_head *node;
	struct proc *proc;
	unsigned long flags;

	spin_lock_irq_save(&(plain_scheduler.proclist.lock), flags);

	if (list_empty(&(plain_scheduler.proclist.head))) {
		spin_unlock_irq_restore(&(plain_scheduler.proclist.lock),
		    flags);
		return NULL;
	}

	/* Involved a trick that we directly start iterating at
	 * plain_scheduler.current, skipping the sentry node. */
	for_each (node, &(plain_scheduler.current->sched_node)) {
		if (node == &(plain_scheduler.proclist.head))
			continue;
		proc = list_entry(node, struct proc, sched_node);
		if (proc->state == PS_RUNNABLE)
			return proc;
	}

	spin_unlock_irq_restore(&(plain_scheduler.proclist.lock), flags);
	return NULL;
}

static void sched_plain_init(void)
{
	plain_scheduler.pick = NULL;
	plain_scheduler.add = NULL;
	plain_scheduler.remove = NULL;
	plain_scheduler.next = NULL;
	plain_scheduler.find = NULL;
}

