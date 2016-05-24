/* Copyright (C) 2016 Xiaofei Bai <xffbai@gmail.com>
 * Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
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

#include <mmu.h>

.section .text.low

/* Broadcast entry points */
.globl master_entry
.globl slave_entry

/*
 * x86 assembly have two syntax: AT&T syntax and Intel syntax.  Linux
 * and GNU toolchain users are probably more familiar with AT&T syntax,
 * while Windows users may get in touch with Intel syntax first.
 *
 * Compared to Intel syntax, AT&T syntax is counter-intuitive in many
 * aspects.  For example,
 *     movl %eax, %ebx
 * means EBX = EAX, rather than EAX = EBX.
 * Another example,
 *     movl 10000, %eax
 * means storing *memory content at address 10000* into register EAX.  To
 * load a number into a register (as intended), you will have to do the
 * following:
 *     movl $10000, %eax
 *
 * Nevertheless, in this project we are sticking to AT&T syntax, since
 * we are using GNU toolchain which prefers AT&T syntax by default, and
 * we don't want to create additional inconsistencies to make things worse.
 *
 * In some comments we would be using Intel syntax notations though, as they
 * are much more readable.  Specifically, [EAX] means memory address EAX.
 *
 * References:
 * https://en.wikibooks.org/wiki/X86_Assembly/GAS_Syntax
 */

master_entry:
	/* Reset state and disable interrupts */
	cli
	/* Set string direction: auto-incrementing */
	cld

	/* Load KERN_BASE for arithmetics */
	#movl	%esi, KERN_BASE

	/*
	 * This piece of code runs at low address, so we should be extremely
	 * cautious about what addresses we are playing with.  Addresses
	 * like _bss_begin and boot_stack are at high address and therefore
	 * should be converted by macro premap_addr to obtain the
	 * corresponding low address.  However, symbols appearing here (e.g.
	 * master_t2) are inherently at low address, so we should *not*
	 * convert them.
	 */

	/* Clear BSS */
	movl	$__premap_addr(_bss_begin), %edi
	movl	$__premap_addr(_bss_end), %ecx
	subl	%edi, %ecx		/* ECX = _bss_end - _bss_begin */
	xor	%eax, %eax
	/*
	 * STOSB stores byte AL at memory address [ES:EDI], and then
	 * increments (or decrements if DF=1, in our case DF is cleared
	 * by CLD instruction) EDI.
	 * REP prefix repeats the instruction ECX times.
	 */
	rep stosb

	/* Lock up early access resource and setup stack */
master_t1:
	movl	$master_t2, %edi
	jmp	lock

master_t2:
	/* Set up stack */
	movl	$__premap_addr(boot_stack), %esp
	#subl	%esp, %esi
	movl	%esp, %ebp

	/* Call into early_init */
	movl	$__premap_addr(master_early_init), %eax
	#subl	%eax, %esi
	call 	*%eax

slave_entry:
	/* Reset state and disable interrupts */
	cli
	/* Set string direction: auto-incrementing */
	cld

	/* Load KERN_BASE for arithmetics */
	#movl	%esi, KERN_BASE

	/* Lock up early access resource and setup stack */
	movl	$slave_t1, %edi
	jmp	lock

slave_t1:
	/* Set up a real stack */
	movl	$__premap_addr(boot_stack), %esp
	#subl	%esp, %esi
	movl	%esp, %ebp

	/* Call into early_init */
	movl	$__premap_addr(slave_early_init), %eax
	#subl	%eax, %esi
	call 	*%eax

lock:
	/* a spin lock */
	xorl	%edx, %edx
	incl	%edx
	movl	$__premap_addr(early_spinlock), %ebx
	#subl	%ebx, %esi
spin_lock_retry:
	xorl	%eax, %eax
	cmpxchgl	%edx, (%ebx)
	jnz	spin_lock_retry

	jmp	*%edi

.globl	master_upper_entry
.text
master_upper_entry:
	movl	$boot_stack, %esp
	movl	$master_init, %eax
	call	*%eax

.bss

	/* Broadcast a pointer the boot data lock, so we can release it later */
.globl early_spinlock

early_spinlock:
	.word	0x0
