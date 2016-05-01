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

#include <sys/types.h>
#include <segment.h>
#include <trap.h>
#include <asm.h>
#include <regs.h>

#define MAX_IDT_ENTRIES	256

struct idtentry idt[MAX_IDT_ENTRIES];
extern generic_fp vectors[];

void setup_idt_entry(struct idtentry *item,
		     bool trap,
		     int selector,
		     void *entry,
		     int dpl)
{
	item->off_lo = (size_t)entry & 0xffff;
	item->cs = selector;
	item->args = 0;
	item->rsv1 = 0;
	item->type = trap ? STS_TG32 : STS_IG32;
}

static const char *trapmsg[] = {
	"Divide by zero",
	"Debug",
	"Non-maskable interrupt",
	"Breakpoint",
	"Overflow",
	"Bounds check",
	"Illegal opcode",
	"Device not available",
	"Double fault",
	"",
	"Invalid task switch segment",
	"Segment not present",
	"Stack exception",
	"General protection fault",
	"Page fault",
	"",
	"Floating point error",
	"Alignment check",
	"Machine check",
	"SIMD error"
};

void trap_init(void)
{
	for (int i = 0; i < MAX_IDT_ENTRIES; ++i)
		setup_idt_entry(&idt[i], 0, SEG_KCODE << 3, vectors[i], 0);

	setup_idt_entry(&idt[T_SYSCALL], 1, SEG_KCODE << 3, vectors[T_SYSCALL],
	    DPL_USER);

	lidt(idt, sizeof(idt));
}

void trap_handler(struct trapframe *tf)
{
	kprintf("Caught exception %d (%s)\n", tf->trapno,
	    tf->trapno <= T_MSG_MAX ? trapmsg[tf->trapno] : "");
	panic("Dying...\n");
}

/*
 * This function is to provide a unified interface for "simulating"
 * the behavior of exiting a trap handler, which is usually the case
 * of finishing fork(2).
 */
__noreturn void trap_return(struct regs *regs)
{
	panic("trap_return\n");
}

void trap_test(void)
{
	int a = 2, b = 5;
	int c = 8;

	for (; b > 0; --b) ;

	c = a / b;
}

