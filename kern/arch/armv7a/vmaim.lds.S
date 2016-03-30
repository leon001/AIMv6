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
	MEM (WX) :
		ORIGIN = 1M,
		LENGTH = MEM_SIZE - 1M
}

SECTIONS
{
	/DISCARD/ : {}

	. = RAM_PHYSBASE;

	. = ALIGN(4K);

	.low_text . : {
		_low_text_begin = .;
		*(.low_text)
		_low_text_end = .;
	}

	. += KERN_BASE;

	. = .;
	_text_begin = .;
	.text . : AT(_text_begin - KERN_BASE) {
		*(.text)
		. = ALIGN(4);
		*(.got) /* Global offset table */
	}
	_text_end = .;
	
	. = .;
	_rodata_begin = .;
	.rodata . : AT(_rodata_begin - KERN_BASE) {
		*(.rodata)
	}
	_rodata_end = .;

	. = .;
	_bss_begin = .;
	.bss . : AT(_bss_begin - KERN_BASE) {
		*(.bss)
	}
	_bss_end = .;

	. = .;
	_data_begin = .;
	.data . : AT(_data_begin - KERN_BASE) {
		*(.data)
	}
	_data_end = .;

	. = ALIGN(16K);

	. = .;
	.boot . : {
		PROVIDE(boot_pgdir = .);
		. += 16K; /* early pgdir */
		. += 4K; /* early stack */
		PROVIDE(boot_stack = .);
	}
}


