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

void trap_init(void)
{
	/* initialize exception vector */
	extern uint32_t arm_vector;
	asm volatile(
		"mcr p15, 0, %[addr], c12, c0, 0;"
		::[addr] "r" (&arm_vector)
	);

	struct regs * regs;

	/* SVC mode */
	regs = kmalloc(sizeof(struct regs), 0);
	if (regs == NULL)
		panic("Failed to allocate SVC context storage.\n");
	kprintf("KERN: SVC context at 0x%08x.\n", regs);
	asm volatile (
		"msr	cpsr_c, %[state1];"
		"mov	sp, %[top];"
		"msr	cpsr_c, %[state2];"
	: /* no output */
	: [top] "r" (regs + 1),
	  [state1] "r" (0xD3),
	  [state2] "r" (0xDF)
	);

	return;
}

void arm_handle_svc(struct regs *regs)
{
	kputs("DEBUG: Enter SVC handler!\n");
	kprintf("DEBUG: r0 = 0x%08x\n", regs->r0);
	kprintf("DEBUG: r1 = 0x%08x\n", regs->r1);
	kprintf("DEBUG: r2 = 0x%08x\n", regs->r2);
	kprintf("DEBUG: r3 = 0x%08x\n", regs->r3);
	kprintf("DEBUG: r4 = 0x%08x\n", regs->r4);
	kprintf("DEBUG: r5 = 0x%08x\n", regs->r5);
	kprintf("DEBUG: r6 = 0x%08x\n", regs->r6);
	kprintf("DEBUG: r7 = 0x%08x\n", regs->r7);
	kprintf("DEBUG: r8 = 0x%08x\n", regs->r8);
	kprintf("DEBUG: r9 = 0x%08x\n", regs->r9);
	kprintf("DEBUG: r10 = 0x%08x\n", regs->r10);
	kprintf("DEBUG: r11 = 0x%08x\n", regs->r11);
	kprintf("DEBUG: r12 = 0x%08x\n", regs->r12);
	kprintf("DEBUG: pc = 0x%08x\n", regs->pc);
	kprintf("DEBUG: psr = 0x%08x\n", regs->psr);
	kprintf("DEBUG: sp = 0x%08x\n", regs->sp);
	kprintf("DEBUG: lr = 0x%08x\n", regs->lr);

	extern void svc_return_asm(struct regs *regs);
	svc_return_asm(regs);
}

void trap_test(void)
{
	asm volatile (
		"svc	0x0;"
	);
}

