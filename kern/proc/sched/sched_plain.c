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
#include <list.h>
#include <mp.h>

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
};

static struct proc *__sched_plain_pick(void);
static int __sched_plain_add(struct proc *proc);
static int __sched_plain_remove(struct proc *proc);
static struct proc *__sched_plain_next(struct proc *proc);

static struct plain_scheduler plain_scheduler = {
	.pick = __sched_plain_pick,
	.add = __sched_plain_add,
	.remove = __sched_plain_remove,
	.next = __sched_plain_next
};

static struct proc *__sched_plain_pick(void)
{
	struct list_head *node;
	struct proc *proc, *picked_proc;
	unsigned long flags;
	spin_lock_irq_save(&(plain_scheduler.proclist.lock), flags);

	if (list_empty(&(plain_scheduler.proclist.head))) {
		picked_proc = NULL;
		goto finalize;
	}

	for_each_entry (proc, &(plain_scheduler.proclist.head), sched_node) {
		if (proc->state == PS_RUNNABLE) {
			/* Move the picked proc to list tail */
			list_del_init(&(proc->sched_node));
			list_add_tail(&(proc->sched_node),
			    &(plain_scheduler.proclist.head));
			picked_proc = proc;
			goto finalize;
		}
	}

	picked_proc = NULL;

finalize:
	spin_unlock_irq_restore(&(plain_scheduler.proclist.lock), flags);
	return picked_proc;
}

static int __sched_plain_add(struct proc *proc)
{
	unsigned long flags;

	spin_lock_irq_save(&(plain_scheduler.proclist.lock), flags);
	list_add_before(&(proc->sched_node), &(plain_scheduler.proclist.head));
	spin_unlock_irq_restore(&(plain_scheduler.proclist.lock), flags);
	return 0;
}

static int __sched_plain_remove(struct proc *proc)
{
	unsigned long flags;

	spin_lock_irq_save(&(plain_scheduler.proclist.lock), flags);
	list_del(&(proc->sched_node));
	spin_unlock_irq_restore(&(plain_scheduler.proclist.lock), flags);

	return 0;
}

static struct proc *__sched_plain_next(struct proc *proc)
{
	struct list_head *head = &(plain_scheduler.proclist.head);

	if (list_empty(head))
		return NULL;
	else if (proc == NULL)
		return list_first_entry(head, struct proc, sched_node);
	else if (list_is_last(&(proc->sched_node), head))
		return NULL;
	else
		return next_entry(proc, sched_node);
}

static int __sched_plain_init(void)
{
	list_init(&(plain_scheduler.proclist.head));
	spinlock_init(&(plain_scheduler.proclist.lock));
	return 0;
}
INITCALL_SCHED(__sched_plain_init);

/* TODO: use a macro switch to enable scheduler module selection */
struct scheduler *scheduler = (struct scheduler *)&plain_scheduler;
