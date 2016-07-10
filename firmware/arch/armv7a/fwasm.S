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

#include <smp.h>

.arm

.section .vector

fw_vector:
	/* Entry */
	ldr	pc, =fw_asm
	/* Interface */
	ldr	pc, =uart_puts
	ldr	pc, =readdisk

.text

fw_asm:
	/* Set state and disable interrupts, but do not touch endianness. */
	msr	cpsr_c, 0xDF

	/* 
	 * Read MPIDR for the multiprocessor ID. Possible results are:
	 * 1. MPIDR is in old format, may indicate old uniprocessor or a MP
	 *    system with those old processors. We currently treat this as
	 *    uniprocessor system.
	 * 2. MPIDR in new format, uniprocessor. We jump to clear BSS.
	 * 3. MPIDR in new format, we're in MP system but not core 0. We go
	 *    into a pen.
	 * 4. MPIDR in new format, we're core 0 in MP system. We go on to
	 *    initialize the kernel. All other cores should be put into
	 *    the wait_for_release loop in THIS FILE.
	 */
	/* Read MPIDR */
	arm_read_mpidr	r0
	/* New format? */
	mov	r1, #0x80000000
	ands	r1, r0, r1
	/* 0 for old format */
	beq	fw_asm_master
	/* MP system? */
	mov	r1, #0x40000000
	ands	r1, r0, r1
	/* 1 for uniprocessor */
	bne	fw_asm_master
	/* In MPCore cluster 0? */
	mov	r1, #0x00000F00
	ands	r1, r0, r1
	bne	fw_asm_slave
	/* Core 0 in MPCore cluster? */
	mov	r1, #0x00000003
	ands	r1, r0, r1
	bne	fw_asm_slave

fw_asm_master:
	/* release other cores from previous boot stage */
	/* we can use the same address. */
	ldr	r0, =#0xFFFFFFF0
	mov	r1, #0
	str	r1, [r0]
	/* Clear BSS */
	ldr	r0, =_bss_begin
	ldr	r1, =_bss_end
	mov	r2, #0x00000000
clbss_l:
	cmp	r0, r1
	/* always use unsigned LOWER */
	strlo	r2, [r0]
	addlo	r0, r0, #4
	blo	clbss_l

	/* Set up stack for firmware and bootloader use */
	ldr	sp, =fw_stack+4096
	movs	fp, sp

	/* And call into firmware code */
	bl	fw_main

fw_asm_slave:
	/* prepare pen */
	ldr	r0, =#0xFFFFFFF0
	mov	r1, #0
	str	r1, [r0]
	/* loop */
wait_for_release:
	wfe
	ldr	r1, [r0]
	cmp	r1, #0
	beq	wait_for_release
	bx	r1

