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

#ifndef _ATTRIBUTES_H
#define _ATTRIBUTES_H

/*
 * Compiler-specific attributes are defined here.
 */

/*
 * Some of the structure/function definitions, e.g. MBR structure
 * definition (because its contents are not aligned to 4 bytes),
 * requires attributes to work.
 */

#ifndef __has_attribute
#define __has_attribute(x)	0
#endif

/*
 * GNU-style attributes are only supported by GCC and LLVM.
 *
 * I wonder if we should use #pragma pack(1), which is supported
 * by most compilers (even those as heretic as Visual Studio supports
 * that).
 */

#if __has_attribute(packed) && __has_attribute(aligned)
#define __packed	__attribute__((packed, aligned(1)))
#elif __has_attribute(packed)
#define __packed	__attribute__((packed))
#else	/* !packed */
#error "Your compiler does not support packed structure?"
#endif	/* packed && aligned */

#endif
