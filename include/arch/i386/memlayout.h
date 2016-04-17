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

#ifndef _MEMLAYOUT_H
#define _MEMLAYOUT_H

/*
 * This file contains description of physical memory layout as well
 * as virtual memory layout, other than what inside config.h
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define BOOT_E820MAP		0x8000
#define BOOT_E820MAP_ERROR	(-1)

#ifndef __ASSEMBLER__

struct e820map {
	uint32_t num;
	__packed struct {
		uint64_t start;
		uint64_t size;
		uint32_t type;
	} map[E820MAX];
};

#endif	/* !__ASSEMBLER__ */

#endif
