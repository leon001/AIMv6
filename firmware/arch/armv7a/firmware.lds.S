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

#define FW_SIZE	1M

OUTPUT_ARCH(arm)
ENTRY(_text_begin)

MEMORY
{
	FIRMWARE (WX) :
		ORIGIN = MEM_SIZE - FW_SIZE,
		LENGTH = FW_SIZE
}

SECTIONS
{
	/DISCARD/ : {}

	. = ALIGN(4K);

	.text : {
		_text_begin = .;
		*(.vector);
		*(.text);
		_text_end = .;
	}
	
	.rodata : {
		*(.rodata);
	}
	
	.bss : {
		_bss_begin = .;
		*(.bss);
		_bss_end = .;
	}

	.data : {
		*(.data);
	}
}
