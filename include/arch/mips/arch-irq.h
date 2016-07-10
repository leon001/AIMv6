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

#ifndef _ASM_IRQ_H
#define _ASM_IRQ_H

#if __mips_isa_rev == 1
/* No revision 2 instructions */
#define local_irq_enable() \
	asm volatile ( \
		".set	push;" \
		".set	noat;" \
		"mfc0	$1, $12;" \
		"ori	$1, 1;" \
		"mtc0	$1, $12;" \
		".set	pop;" \
		: /* no output */ \
		: /* no input */ \
		: "$1", "memory" \
	)

#define local_irq_disable() \
	asm volatile ( \
		".set	push;" \
		".set	noat;" \
		"mfc0	$1, $12;" \
		"ori	$1, 1;" \
		"xori	$1, 1;" \
		"mtc0	$1, $12;" \
		".set	pop;" \
		: /* no output */ \
		: /* no input */ \
		: "$1", "memory" \
	)

#define local_irq_save(flags) \
	asm volatile ( \
		".set	push;" \
		".set	noat;" \
		"mfc0	%0, $12;" \
		"ori	$1, %0, 0x1f;" \
		"xori	$1, $1, 0x1f;" \
		"mtc0	$1, $12;" \
		".set	pop;" \
		: "=r"(flags) \
		: /* no input */ \
		: "$1", "memory" \
	)

#define local_irq_restore(flags) \
	asm volatile ( \
		"	.set	push;" \
		"	.set	reorder;" \
		"	.set	noat;" \
		"	mfc0	$1, $12;" \
		"	andi	%0, 1;" \
		"	ori	$1, 0x1f;" \
		"	xori	$1, 0x1f;" \
		"	or	%0, $1;" \
		"	mtc0	%0, $12;" \
		"	.set	pop;" \
		: /* no output */ \
		: "r"(flags) \
		: "$1", "memory" \
	)
#else	/* Rev 2 */
#define local_irq_enable()	asm volatile ("ei")

#define local_irq_disable()	asm volatile ("di")

#define local_irq_save(flags) \
	asm volatile ( \
		"mfc0	%0, $12;" \
		"di;" \
		"andi	%0, 1;" \
		: "=r"(flags) \
		: /* no input */ \
		: "memory" \
	)

#define local_irq_restore(flags) \
	asm volatile ( \
		"	beqz	%0, 1f;" \
		"	di;" \
		"	ei;" \
		"1:" \
		: /* no output */ \
		: "r"(flags) \
		: "memory" \
	)
#endif

#endif
