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
#include <stack.h>
#include <mmu.h>
#include <regs.h>
#include <proc.h>
#include <percpu.h>

static struct trapframe *__proc_trapframe(struct proc *proc)
{
	struct trapframe *tf;

	tf = (struct trapframe *)(proc->kstack + KSTACKSIZE - sizeof(*tf));
	return tf;
}

/******** Kernel processes *******/
static void __bootstrap_ktrapframe(struct trapframe *tf, void *entry, void *stack)
{
	tf->status = read_c0_status();
	tf->cause = read_c0_cause();
	tf->epc = (unsigned long)entry;
	tf->gpr[_T9] = tf->epc;
	tf->gpr[_SP] = (unsigned long)stack;
}

extern void forkret(void);

static void __bootstrap_kcontext(struct regs *regs, struct trapframe *tf)
{
	regs->status = read_c0_status();
	regs->cause = read_c0_cause();
	/* t9 is the register storing function entry address in PIC */
	regs->gpr[_T9] = regs->gpr[_RA] = (unsigned long)forkret;
	/* Kernel stack pointer just below trap frame */
	regs->gpr[_SP] = (unsigned long)tf;
}

void proc_ksetup(struct proc *proc,
		void *entry,
		void *stack,
		int argc,
		char *argv[],
		char *envp[])
{
	struct trapframe *tf = __proc_trapframe(proc);
	__bootstrap_ktrapframe(tf, entry, stack);
	__bootstrap_kcontext(&(proc->context), tf);
}

/******** User processes (TODO) *******/

void proc_trap_return(struct proc *proc)
{
	struct trapframe *tf = __proc_trapframe(proc);

	trap_return(tf);
}

void switch_context(struct proc *proc)
{
	struct proc *current = current_proc;
	current_proc = proc;

	/* Switch page directory */
	pgdir_slots[cpuid()] = proc->mm->pgindex;
	/* Switch kernel stack */
	kernelsp[cpuid()] = (unsigned long)(proc->kstack + KSTACKSIZE);
	/* Switch general registers */
	switch_regs(&(current->context), &(proc->context));
}

