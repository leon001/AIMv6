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

#ifndef _MIPS32_IO_H
#define _MIPS32_IO_H

#include <sys/types.h>
#include <addrspace.h>

#define iomap(addr)		(TO_UNCAC(addr))

/*
 * in8() and out8() for MIPS receives physical address as parameter
 */

#define read8(addr)		\
	(*(volatile uint8_t *)iomap((unsigned long)(addr)))
#define write8(addr, data)	\
	((*(volatile uint8_t *)iomap((unsigned long)(addr))) = (data))

#define read16(addr)		\
	(*(volatile uint16_t *)iomap((unsigned long)(addr)))
#define write16(addr, data)	\
	((*(volatile uint16_t *)iomap((unsigned long)(addr))) = (data))

#define read32(addr)		\
	(*(volatile uint32_t *)iomap((unsigned long)(addr)))
#define write32(addr, data)	\
	((*(volatile uint32_t *)iomap((unsigned long)(addr))) = (data))

#endif
