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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

OUTPUT_ARCH(arm)
ENTRY(master_entry)

MEMORY
{
	PMEM (WX) :
		ORIGIN = RAM_PHYSBASE,
		LENGTH = MEM_SIZE - 1M
	VMEM (WX) :
		ORIGIN = KERN_BASE,
		LENGTH = MEM_SIZE - 1M
}

SECTIONS
{
	/DISCARD/ : {}

	.low_text : {
		_low_text_begin = .;
		*(.low_text)
		_low_text_end = .;
	} >PMEM AT>PMEM

	.text : {
		_text_begin = .;
		*(.text)
		_text_end = .;
	} >VMEM AT>PMEM

	.got : {
		*(.got)
	} >VMEM AT>PMEM

	.rodata : {
		_rodata_begin = .;
		*(.rodata)
		_rodata_end = .;
	} >VMEM AT>PMEM

	.data : {
		_data_begin = .;
		*(.data)
		_data_end = .;
	} >VMEM AT>PMEM

	.bss : {
		_bss_begin = .;
		*(.bss)
		_bss_end = .;
	} >VMEM AT>PMEM

	.boot ALIGN(16K) : {
		PROVIDE(boot_pgdir = .);
		. += 16K; /* early pgdir */
		. += 4K; /* early stack */
		PROVIDE(boot_stack = .);
	} >VMEM AT>PMEM
}


