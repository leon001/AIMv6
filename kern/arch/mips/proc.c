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

#include <arch-trap.h>
#include <mipsregs.h>

static void __bootstrap_trapframe(struct trapframe *tf, void *entry, void *stack)
{
	tf->status = read_c0_status();
	tf->cause = read_c0_cause();
	tf->epc = (unsigned long)entry;
	tf->gpr[_SP] = (unsigned long)stack;
}

extern void forkret(void);

static void __bootstrap_context(struct regs *regs, struct trapframe *tf)
{
	regs->gpr[_RA] = forkret;
	/* Kernel stack pointer just below trap frame */
	regs->gpr[_SP] = tf;
}

void proc_setup(struct proc *proc, void *entry, void *stack)
{
	struct trapframe *tf;

	tf = (struct trapframe *)(proc->kstack - sizeof(*tf));
	__bootstrap_trapframe(tf, entry, stack);
	__bootstrap_context(&(proc->context), tf);
}

void switch_context(struct proc *proc)
{
	struct proc *current = current_proc;
	current_proc = proc;

	/* Switch page directory */
	pgdir_slots[cpuid()] = proc->mm->pgindex;
	/* Switch kernel stack */
	kernelsp[cpuid()] = proc->kstack + KSTACK_SIZE;
	/* Switch general registers */
	switch_regs(&(current->context), &(proc->context));
}

