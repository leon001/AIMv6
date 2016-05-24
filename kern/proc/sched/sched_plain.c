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
#include <sched.h>
#include <bitmap.h>
#include <namespace.h>
#include <aim/sync.h>
#include <aim/initcalls.h>

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

static struct proc *__sched_plain_pick(void);
static int __sched_plain_add(struct proc *proc);

static struct plain_scheduler plain_scheduler = {
	.pick = __sched_plain_pick,
	.add = __sched_plain_add
};

static struct proc *__sched_plain_pick(void)
{
	struct list_head *node;
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
	for_each (node,
	    !plain_scheduler.current ?
	    &(plain_scheduler.proclist.head) :
	    &(plain_scheduler.current->sched_node)) {
		if (node == &(plain_scheduler.proclist.head))
			continue;
		proc = list_entry(node, struct proc, sched_node);
		if (proc->state == PS_RUNNABLE) {
			plain_scheduler.current = proc;
			spin_unlock_irq_restore(
			    &(plain_scheduler.proclist.lock),
			    flags
			);
			return proc;
		}
	}

	spin_unlock_irq_restore(&(plain_scheduler.proclist.lock), flags);
	return NULL;
}

static int __sched_plain_add(struct proc *proc)
{
	unsigned long flags;

	spin_lock_irq_save(&(plain_scheduler.proclist.lock), flags);
	list_add_before(&(proc->sched_node), &(plain_scheduler.proclist.head));
	spin_unlock_irq_restore(&(plain_scheduler.proclist.lock), flags);
	return 0;
}

static int __sched_plain_init(void)
{
	list_init(&(plain_scheduler.proclist.head));
	spinlock_init(&(plain_scheduler.proclist.lock));
	plain_scheduler.current = NULL;
	return 0;
}
INITCALL_SUBSYS(__sched_plain_init);

/* TODO: use a macro switch to enable scheduler module selection */
struct scheduler *scheduler = (struct scheduler *)&plain_scheduler;
