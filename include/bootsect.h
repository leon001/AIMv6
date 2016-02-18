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

#ifndef _BOOTSECT_H
#define _BOOTSECT_H

#include <libc/sys/types.h>
#include <attributes.h>

/*
 * IBM PC-compatible MBR structure
 */

struct chs {
	unsigned char	head;
	unsigned char	sector:6;
	unsigned char	cylinder_hi:2;
	unsigned char	cylinder_lo;
};

struct mbr_part_entry {
	uint8_t		status;
#define BOOTABLE	0x80
#define INACTIVE	0x00
	struct chs	first_sector_chs;
	uint8_t		type;
	struct chs	last_sector_chs;
	uint32_t	first_sector_lba;
	uint32_t	sector_count;
};

#define BOOTLOADER_SIZE		446
#define MAX_PRIMARY_PARTITIONS	4

struct mbr {
	unsigned char	code[BOOTLOADER_SIZE];
	struct mbr_part_entry part_entry[MAX_PRIMARY_PARTITIONS];
	unsigned char	signature[2];
} __packed;

#endif
