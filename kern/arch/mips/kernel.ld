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

OUTPUT_FORMAT("elf32-tradlittlemips")
ENTRY(__start)

SECTIONS {
	. = 0x80300000;
	.text : {
		*(.text*);
	}
	.rodata : {
		*(.rodata*);
	}
	.data : {
		*(.data*);
	}
	.bss : {
		PROVIDE(__bss_begin__ = .);
		*(.bss*);
		PROVIDE(__bss_end__ = .);
	}
	PROVIDE(__bss_size__ = __bss_end__ - __bss_begin__);
}
