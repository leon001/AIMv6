/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _MIPS32_IO_H
#define _MIPS32_IO_H

#include <libc/sys/types.h>
#include <addrspace.h>

#define iomap(addr)		(TO_UNCAC(addr))

/*
 * in8() and out8() for MIPS receives physical address as parameter
 */
#define read8(addr)		\
	(*(volatile uchar *)iomap(addr))
#define write8(addr, data)	\
	((*(volatile uchar *)iomap(addr)) = (data))

#define read16(addr)		\
	(*(volatile ushort *)iomap(addr))
#define write16(addr, data)	\
	((*(volatile ushort *)iomap(addr)) = (data))

#define read32(addr)		\
	(*(volatile uint *)iomap(addr))
#define write32(addr, data)	\
	((*(volatile uint *)iomap(addr)) = (data))

#endif
