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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <mm.h>
#include <vmm.h>
#include <panic.h>
#include <console.h>
#include <regs.h>

#include <arm-trap.h>

static void arm_init_mode(uint32_t psr_c, char *name)
{
	struct regs * regs;

	regs = kmalloc(sizeof(struct regs), 0);
	if (regs == NULL)
		panic("Failed to allocate %s context storage.\n", name);
	kprintf("KERN: %s context at 0x%08x.\n", name, regs);
	asm volatile (
		"msr	cpsr_c, %[state1];"
		"mov	sp, %[top];"
		"msr	cpsr_c, %[state2];"
	: /* no output */
	: [top] "r" (regs + 1),
	  [state1] "r" (psr_c),
	  [state2] "r" (0xDF)
	);
}

void trap_init(void)
{
	/* initialize exception vector */
	extern uint32_t arm_vector;
	asm volatile(
		"mcr p15, 0, %[addr], c12, c0, 0;"
		::[addr] "r" (&arm_vector)
	);

	/* IRQ mode */
	arm_init_mode(0xD1, "IRQ");

	/* FIQ(0xD2) mode not used */

	/* SVC mode */
	arm_init_mode(0xD3, "SVC");

	/* ABT mode */
	arm_init_mode(0xD7, "ABT");

	/* UNDEF mode */
	arm_init_mode(0xDB, "UNDEF");

	return;
}

__noreturn
void trap_return(struct regs *regs)
{
	/*
	 * You MUST be in SYS mode to call this routine.
	 * regs MUST NOT be on the heap UNLESS it IS exactly the one passed
	 * in arm_handle_trap and IRQ is never enabled. This routine will not
	 * free the pointer.
	 * This routine returns to the execution state in regs and all further
	 * stack use are discarded.
	 * If this current exception interrupts a kernel control flow, the
	 * previous stack state is fully recovered.
	 */
	asm volatile (
		/* disable interrupts */
		"msr	cpsr_c, 0xDF;"
		/* restore banked registers */
		"ldmia	r0!, {sp, lr};"
		/* go to SVC mode */
		"msr	cpsr_c, 0xD3;"
		/* restore other registers */
		"ldmia	r0!, {r1};"
		"msr	spsr, r1;"
		"ldmia	r0, {r0-r12, pc}^;"
		/* the instruction above performs the exception return */
	);
	panic("Control flow went beyond trap_return().");
}

__noreturn
void arm_handle_trap(struct regs *regs, uint32_t type)
{
	/*
	 * We're here in SYS mode with IRQ disabled.
	 * type contains the trap type, see arm-trap.h for details
	 * regs is stored in per-CPU per-MODE storage.
	 * you MUST store it somewhere else before you can turn on
	 * IRQ again - a further exception will reuse that.
	 * you MUST NOT try to free that memory.
	 * you are RECOMMENDED to store regs on stack and not on heap.
	 * see trap_return for details.
	 */
	kpdebug("Enter vector slot %d handler!\n", type);
	kpdebug("r0 = 0x%08x\n", regs->r0);
	kpdebug("r1 = 0x%08x\n", regs->r1);
	kpdebug("r2 = 0x%08x\n", regs->r2);
	kpdebug("r3 = 0x%08x\n", regs->r3);
	kpdebug("r4 = 0x%08x\n", regs->r4);
	kpdebug("r5 = 0x%08x\n", regs->r5);
	kpdebug("r6 = 0x%08x\n", regs->r6);
	kpdebug("r7 = 0x%08x\n", regs->r7);
	kpdebug("r8 = 0x%08x\n", regs->r8);
	kpdebug("r9 = 0x%08x\n", regs->r9);
	kpdebug("r10 = 0x%08x\n", regs->r10);
	kpdebug("r11 = 0x%08x\n", regs->r11);
	kpdebug("r12 = 0x%08x\n", regs->r12);
	kpdebug("pc = 0x%08x\n", regs->pc);
	kpdebug("psr = 0x%08x\n", regs->psr);
	kpdebug("sp = 0x%08x\n", regs->sp);
	kpdebug("lr = 0x%08x\n", regs->lr);

	trap_return(regs);
}

void trap_test(void)
{
	asm volatile (
		"svc	0x0;"
	);
}

