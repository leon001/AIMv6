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

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#ifndef __ASSEMBLER__
/*
 * This macro is a wrapper for casting integer constants to unsigned longs,
 * freeing the code from compiler warnings and assembler errors (GCC
 * warns about shift count if a "UL" suffix is not appended while GAS
 * refuses to recognize the "UL" suffix).
 */
#define ULCAST(i)	(i##UL)
#else	/* __ASSEMBLER__ */
#define ULCAST(i)	i
#endif	/* !__ASSEMBLER__ */

#ifndef __ASSEMBLER__
typedef unsigned char uint8, uchar, byte, __u8, u8, uint8_t;
typedef signed char __s8, int8_t;
typedef unsigned short uint16, ushort, __u16, u16, uint16_t;
typedef signed short __s16, int16_t;
typedef unsigned int uint32, uint, __u32, u32, uint32_t;
typedef signed int __s32, int32_t;
typedef unsigned long ulong;
typedef unsigned long long uint64, __u64, u64, uint64_t;
typedef signed long long int64, __s64, int64_t;
typedef unsigned int bool;
#define false	0
#define true	1

#define	NULL	0
#define EOF	-1

typedef unsigned long size_t;
typedef signed long ssize_t;

typedef void *uintptr_t;

/* A generic void function pointer type, allow any number of arguments */
typedef void (*generic_fp)();

#endif

#endif

