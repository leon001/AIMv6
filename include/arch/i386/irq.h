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

#include <processor-flags.h>

#define local_save_flags(flags) \
	asm volatile ("pushf; pop %0;" : "=rm"(flags) : : "memory")

#define local_restore_flags(flags) \
	asm volatile ("push %0; popf;" : : "g"(flags) : "memory")

#define local_irq_enable() \
	asm volatile ("sti;" : : : "memory")

#define local_irq_disable() \
	asm volatile ("cli;" : : : "memory")

#define local_irq_save(flags) \
	do { \
		local_save_flags(flags); \
		local_irq_disable(); \
	} while (0)

#define local_irq_restore(flags) \
	do { \
		local_restore_flags(flags); \
	} while (0)

#endif
