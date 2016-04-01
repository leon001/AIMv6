/* Copyright (C) 2016 Xiaofei Bai <xffbai@gmail.com>
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

.section .entry

/* Broadcast entry points */
.globl master_entry
.globl slave_entry

master_entry:
	/* Reset state and disable interrupts */
	cli
	/* Set string direction: auto-incrementing */
	cld

	/* Load KERN_BASE for arithmetics */
	movl	%esi, KERN_BASE

	/* Clear BSS */
	movl	%eax, _bss_begin
	subl	%eax, %esi
	movl	%edx, _bss_end
	subl	%edx, %esi
	movl	%ecx, 0x00000000
clbss_l:
	cmpl	%eax, %edx
	jge	master_t1
	movl	(%eax), %ecx
	addl	%eax, 4
	jmp	clbss_l

	/* Lock up early access resource and setup stack */
master_t1:
	movl	%edi, master_t2
	jmp	lock

master_t2:
	/* Set up stack */
	movl	%esp, boot_stack
	subl	%esp, %esi
	movl	%ebp, %esp

	/* Call into early_init */
	movl	%eax, master_early_init
	subl	%eax, %esi
	call 	*%eax

slave_entry:
	/* Reset state and disable interrupts */
	cli
	/* Set string direction: auto-incrementing */
	cld

	/* Load KERN_BASE for arithmetics */
	movl	%esi, KERN_BASE

	/* Lock up early access resource and setup stack */
	movl	%edi, slave_t1
	jmp	lock

slave_t1:
	/* Set up a real stack */
	movl	%esp, boot_stack
	subl	%esp, %esi
	movl	%ebp, %esp

	/* Call into early_init */
	movl	%eax, slave_early_init
	subl	%eax, %esi
	call 	*%eax

lock:
	/* a spin lock */
	xorl	%edx, %edx
	incl	%edx
	movl	%ebx, early_spinlock
	subl	%ebx, %esi
spin_lock_retry:
	xorl	%eax, %eax
	cmpxchgl	%edx, (%ebx)
	jnz	spin_lock_retry

	jmp	*%edi

.bss

	/* Broadcast a pointer the boot data lock, so we can release it later */
.globl early_spinlock

early_spinlock:
	.word	0x0
