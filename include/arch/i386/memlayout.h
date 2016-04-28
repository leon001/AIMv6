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

#define E820_RAM		1	/* Usable RAM */
#define E820_RESERVED		2	/* Unusable */
#define E820_RECLAIMABLE	3	/* ACPI reclaimable */
#define E820_NVS		4	/* ACPI NVS */
#define E820_BAD		5	/* Bad memory */

#define E820MAX			20

#define HIGHMEM_BASE		0x100000

#ifndef __ASSEMBLER__

#pragma pack(1)
struct e820entry {
	uint64_t start;
	uint64_t size;
	uint32_t type;
	/*
	 * Quoting OSDev:
	 * The format of an entry is 2 uint64_t's and a uint32_t
	 * in the 20 byte version, plus one additional uint32_t
	 * in the 24 byte ACPI 3.0 version (but nobody has ever
	 * seen a 24 byte one)
	 */
	uint32_t reserved;
};

struct e820map {
	uint32_t num;
	struct e820entry map[E820MAX];
};

#pragma pack()

#endif	/* !__ASSEMBLER__ */

#endif
