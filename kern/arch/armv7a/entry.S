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

.text

master_entry:
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

	/* Lock up early access resource and setup stack */
	bl	lock
	/* 
	 * The above subroutine call is safe. ARM's ISA DOES NOT use the stack
	 * during calls or returns. It is up to the callee whether to save
	 * the return address somewhere.
	 *
	 * We are calling a leaf routine, and it is hand-written so it doesn't
	 * save anything on stack.
	 */
	ldr	sp, =boot_stack
	movs	fp, sp

	/* Call into early_init */
	bl	master_early_init

/* Broadcast a entry pointer to start slave cores later */
.globl slave_entry

slave_entry:
	/* Slave cores also set states, but DO NOT clear bss */
	msr	cpsr_c, 0xDF

	/* Lock up early access resource and setup stack */
	bl	lock
	/* The above subroutine call is safe. See explainations above. */
	ldr	sp, =boot_stack
	movs	fp, sp

	/* Call into early_init */
	bl	slave_early_init

lock:
	mov	r0, #1			/* prepare a non-zero value */
	ldr	r1, =early_spinlock	/* load lock address */
	ldrex	r5, [r1]		/* load lock value */
	cmp	r5, #0
	wfene				/* sleep if locked */
	strexeq	r5, r0, [r1]		/* if free, try to lock it */
	cmpeq	r5, #0
	bne 	early_loop		/* try again if failed */
	dmb				/* data memory barrier */
	bx	lr			/* return */

.bss

/* Broadcast a pointer the boot data lock, so we can release it later */
.globl early_spinlock

early_spinlock:
	.word	0x0

