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

/*
 * These code are for firmware development in MSIM.
 * Loongson developers don't need to deal with firmware; Loongson already
 * shipped a powerful, reliable one in those boxes.
 */

#include <asm.h>
#include <regdef.h>
#include <cp0regdef.h>
#include <smp.h>
#include <addrspace.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

.section .text.reset

BEGIN(reset)
	/* Turn off all interrupts, and go into kernel mode */
	MFC032	a0, CP0_STATUS
	or	a0, ST_EXCM
	xor	a0, ST_EXCM
	MTC032	a0, CP0_STATUS
	/*
	 * Obtain the current CPU ID.
	 * Note that `cpuid' is *not* a MIPS instruction.  This is a
	 * pseudo-instruction defined in include/arch/mips/smp.h.
	 *
	 * The CPU ID is stored in register a1, a2 is a temporary
	 * register unused here.  The reason I let this pseudo-instruction
	 * accept two registers is to provide flexibility for other
	 * MIPS Revision 1 platforms where one register is not sufficient.
	 */
	cpuid	a1, a2
	/*
	 * If the CPU ID is zero, select this CPU to be master.  All other
	 * CPUs are slaves which busy-waits until the master initialized
	 * everything necessary for bringing them up.
	 */
	bnez	a1, 2f
	j	master_entry
2:	j	slave_hangup
END(reset)

.text

	.align	9		/* 0xbfc00200 */
BEGIN(__tlbfill)
	j	__exception_entry
END(__tlbfill)

	.align	8		/* 0xbfc00300 */
BEGIN(__cacheerror)
	j	__exception_entry
END(__cacheerror)

	.align	7		/* 0xbfc00380 */
BEGIN(__exception_entry)
	j	__exception_entry
END(__exception_entry)

BEGIN(slave_hangup)
	/*
	 * We configured MSIM in msim.conf so that each CPU has one
	 * "own" mailbox.  The size of such mailbox is 16 bytes.
	 * After the master has setup everything (e.g. trap handlers
	 * and devices), the master writes the entry address for slaves
	 * to jump into, in each mailbox.
	 *
	 * This is actually what Loongson 3A is doing.  In fact, each
	 * Loongson 3A core has 4 64-bit mailboxes.  In order to bring
	 * up slaves, the master writes the entry address, the stack pointer
	 * value (sp), the global pointer value (gp) and the argument for
	 * such entry into those mailboxes.  Here, MSIM is modelling Loongson
	 * 3A's behavior.
	 */
	/* a1 already contains cpuid */
	SLL	t0, a1, MSIM_ORDER_MAILBOX_ORDER
	STORE	zero, TO_UNCAC(MSIM_ORDER_MAILBOX_BASE)(t0)
1:	LOAD	t1, TO_UNCAC(MSIM_ORDER_MAILBOX_BASE)(t0)
	beqz	t1, 1b
	jr	t1
END(slave_hangup)

BEGIN(master_entry)
	/*
	 * A problem for firmware development is that, the firmware blob
	 * resides in read-only, non-volatile memory, and therefore,
	 * writing to global data is impossible without special treatments.
	 * To resolve this issue, we need to generate code as if the variables
	 * are inside RAM, but actually put the initialization values for
	 * those variables inside ROM.  When the firmware is executed, we
	 * relocate the global data into RAM as soon as RAM is initialized
	 * (in case of MSIM, the RAM is already initialized).
	 *
	 * We know from the linker script that the global data section
	 * ".data", containing global variables, comes after the read-only data
	 * section ".rodata", usually containing constant strings.  As soon
	 * as RAM is initialized, we relocate the content for ".data" section
	 * from firmware to where they really should be.
	 */
reloc_data:
	LA	s0, __rodata_end__
	LA	s1, __data_begin__
	LA	s2, __data_end__
1:	bge	s1, s2, 9f
	lw	t0, (s0)
	sw	t0, (s1)
	ADDU	s0, 4
	ADDU	s1, 4
	b	1b
9:
	/*
	 * We shall deal with global, zero-initialized variables, which reside
	 * in section ".bss".  A global, zero-initialized variable is a global
	 * variable without initialization, and therefore is initialized by
	 * default (i.e. zeros).
	 *
	 * However, since the content in RAM is usually random, we need to
	 * clean the content there by our own.
	 *
	 * Because we allocated per-CPU stacks as a two-dimensional array
	 * (see firmware/arch/mips/msim/main.c) without initialization (hence
	 * being a part of ".bss"), we need to clean the content there before
	 * using the stack, and therefore before jumping to C code, because
	 * C code requires a stack to work.  This is why we did data relocation
	 * and ".bss" cleaning in assembly here.
	 */
init_bss:
	LA	s1, __bss_begin__
	LA	s2, __bss_end__
1:	bge	s1, s2, 9f
	sw	zero, (s1)
	ADDU	s1, 4
	b	1b
9:
	/*
	 * Stacks on MIPS grows down.  When "pushing" onto the stack, the stack
	 * pointer is decreased first, then the contents are written onto the
	 * stack.
	 */
init_master_stack:
	ADDU	a1, 1
	SLL	a3, a1, FWSTACKORDER
	LA	sp, fwstack(a3)
main_entry:
	j	main
END(master_entry)
