/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
 * Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
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
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <mm.h>
#include <pmm.h>
#include <proc.h>
#include <namespace.h>
#include <libc/string.h>

/*
 * This should be a seperate function, don't directly use kernel memory
 * interfaces. There are different sets of interfaces we can allocate memory
 * from, and we can't say any one of them is best.
 */
void *alloc_kstack(void)
{
	/* currently we use pgalloc() */
	addr_t paddr;

	paddr = pgalloc();
	if (paddr == -1)
		return NULL;
	return pa2kva(paddr);
}

void *alloc_kstack_size(size_t *size)
{
	/* calculate the actual size we allocate */
	panic("custom kstack size not implemented\n");
}

struct proc *proc_new(struct namespace *ns)
{
	struct proc *proc = (struct proc *)kmalloc(sizeof(*proc), 0);

	proc->kstack = alloc_kstack();
	proc->kstack_size = PAGE_SIZE;

	proc->tid = 0;
	proc->kpid = kpid_new();
	proc->pid = (ns == NULL) ? proc->kpid : pid_new(ns);
	proc->state = PS_EMBRYO;
	proc->exit_code = 0;
	proc->exit_signal = 0;
	proc->flags = 0;
	proc->oncpu = CPU_NONE;
	proc->bed = NULL;
	proc->namespace = ns;
	proc->mm = mm_new();
	memset(&(proc->context), 0, sizeof(&(proc->context)));
	proc->heapsize = 0;
	proc->ustacktop = 0;
	proc->progtop = 0;
	memset(&(proc->name), 0, sizeof(&(proc->name)));

	proc->parent = NULL;
	proc->first_child = NULL;
	proc->next_sibling = NULL;
	proc->prev_sibling = NULL;
	list_init(&(proc->proc_node));
}

