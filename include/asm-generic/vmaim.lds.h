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

#ifndef _ASM_GENERIC_VMAIM_LDS_H
#define _ASM_GENERIC_VMAIM_LDS_H

#include <aim/export.h>

#define LOAD_OFFSET	(KERN_BASE - KERN_PHYSBASE)

#define AT_OFFSET(sec)	AT(ADDR(sec) - LOAD_OFFSET)

/* 8 byte is the maximum alignment a function may require. */
#define ALIGN_FUNCTION() \
	. = ALIGN(8)

/* And a structure alignment as well */
#define STRUCT_ALIGNMENT 32
#define STRUCT_ALIGN() . = ALIGN(STRUCT_ALIGNMENT)

#define HIGH_SECTION(sec) \
	sec : AT_OFFSET(sec)

#define TEXT								\
	ALIGN_FUNCTION();						\
	*(.text)							\

#define TEXT_LOW							\
	ALIGN_FUNCTION();						\
	*(.text.low)

#define DATA(align)							\
	. = ALIGN((align));						\
	*(.data)

#define RODATA(align)							\
	. = ALIGN((align));						\
	*(.rodata)

#define BSS(align)							\
	. = ALIGN((align));						\
	*(.bss)								\
	*(COMMON)

#endif /* _ASM_GENERIC_VMAIM_LDS_H */

