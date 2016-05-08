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

#include <libc/string.h>
#include <sys/types.h>
#include <regs.h>
#include <mipsregs.h>
#include <cp0regdef.h>
#include <trap.h>
#include <console.h>
#include <arch-trap.h>

void trap_init(void)
{
	extern uint32_t generic_exception_entry;
	memcpy((void *)GENERIC_EXCEPT_ENTRY, &generic_exception_entry, 0x80);

	uint32_t status = read_c0_status();
	write_c0_status(status & ~ST_BEV);
}

static void dump_regs(struct regs *regs)
{
	kprintf("zero\t%016x\n", regs->gpr[_ZERO]);
	kprintf("at\t%016x\n", regs->gpr[_AT]);
	kprintf("v0\t%016x\n", regs->gpr[_V0]);
	kprintf("v1\t%016x\n", regs->gpr[_V1]);
	kprintf("a0\t%016x\n", regs->gpr[_A0]);
	kprintf("a1\t%016x\n", regs->gpr[_A1]);
	kprintf("a2\t%016x\n", regs->gpr[_A2]);
	kprintf("a3\t%016x\n", regs->gpr[_A3]);
	kprintf("t0\t%016x\n", regs->gpr[_T0]);
	kprintf("t1\t%016x\n", regs->gpr[_T1]);
	kprintf("t2\t%016x\n", regs->gpr[_T2]);
	kprintf("t3\t%016x\n", regs->gpr[_T3]);
	kprintf("t4\t%016x\n", regs->gpr[_T4]);
	kprintf("t5\t%016x\n", regs->gpr[_T5]);
	kprintf("t6\t%016x\n", regs->gpr[_T6]);
	kprintf("t7\t%016x\n", regs->gpr[_T7]);
	kprintf("s0\t%016x\n", regs->gpr[_S0]);
	kprintf("s1\t%016x\n", regs->gpr[_S1]);
	kprintf("s2\t%016x\n", regs->gpr[_S2]);
	kprintf("s3\t%016x\n", regs->gpr[_S3]);
	kprintf("s4\t%016x\n", regs->gpr[_S4]);
	kprintf("s5\t%016x\n", regs->gpr[_S5]);
	kprintf("s6\t%016x\n", regs->gpr[_S6]);
	kprintf("s7\t%016x\n", regs->gpr[_S7]);
	kprintf("t8\t%016x\n", regs->gpr[_T8]);
	kprintf("t9\t%016x\n", regs->gpr[_T9]);
	kprintf("gp\t%016x\n", regs->gpr[_GP]);
	kprintf("sp\t%016x\n", regs->gpr[_SP]);
	kprintf("s8\t%016x\n", regs->gpr[_S8]);
	kprintf("ra\t%016x\n", regs->gpr[_RA]);
	kprintf("LO\t%016x\n", regs->lo);
	kprintf("HI\t%016x\n", regs->hi);
	kprintf("STATUS\t%016x\n", regs->status);
	kprintf("CAUSE\t%016x\n", regs->cause);
	kprintf("EPC\t%016x\n", regs->epc);
	kprintf("BVA\t%016x\n", regs->badvaddr);
}

void trap_handler(struct regs *regs)
{
	dump_regs(regs);
	panic("Unexpected trap\n");
	trap_return(regs);
}

extern void trap_exit(struct regs *regs);

__noreturn void trap_return(struct regs *regs)
{
	/* Retain interrupt masks while changing other fields according to
	 * register set */
	uint32_t status = read_c0_status();
	uint32_t im = status & ST_IM;
	status &= ~ST_EXCM;
	write_c0_status(status);
	regs->status = (regs->status & ~ST_IM) | im;
	trap_exit(regs);
}

void trap_test(void)
{
}

