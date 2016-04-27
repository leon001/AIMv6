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

.arm

.section .text.vector

.globl arm_vector

arm_vector:
	ldr	pc, = idle	//Reset
	ldr	pc, = idle	//Undefined
	ldr	pc, = svc_handler_asm	//SVC
	ldr	pc, = idle	//Prefetch Abort
	ldr	pc, = idle	//Data Abort
	ldr	pc, = idle	//Reserved
	ldr	pc, = idle	//IRQ
	ldr	pc, = idle	//FIQ

.text

svc_handler_asm:
	/* push registers */
	push	{r0-r12, lr}
	mrs	r0, spsr
	push	{r0}
	/* pass a pointer using r0 */
	mov	r0, sp
	/* restore stack pointer */
	add	sp, sp, #0x3C
	/* go to system mode and save banked registers */
	msr	cpsr_c, 0xDF
	stmdb	r0!, {sp, lr}
	/* initialize or reuse stack */
	/* TODO */

	/* call the C handler */
	bl	arm_handle_svc
	/* arm_handle_svc(struct regs *) */

idle:
	b	idle

