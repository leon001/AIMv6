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

