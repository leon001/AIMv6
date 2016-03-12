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
ENTRY(reset)

MEMORY {
	rom (RX) : ORIGIN = 0xbfc00000, LENGTH = 1M
	ram (RW) : ORIGIN = 0x80000000, LENGTH = 256M
}

SECTIONS {
	.text : {
		__text_begin__ = .;
		*(.text.reset);
		*(.text*);
		__text_end__ = .;
	} > rom

	.rodata : {
		__rodata_begin__ = .;
		*(.rodata*);
		__rodata_end__ = .;
	} > rom

	PROVIDE(__rodata_size__ = SIZEOF(.rodata));

	.data : AT(__rodata_end__) {
		__data_begin__ = .;
		*(.data);
		__data_end__ = .;
	} > ram
	PROVIDE(__data_size__ = SIZEOF(.data));

	.bss : {
		__bss_begin__ = .;
		*(.bss);
		__bss_end__ = .;
	}
	PROVIDE(__bss_size__ = SIZEOF(.bss));

	/DISCARD/ : {
		*(.reginfo);
		*(.MIPS.abiflags);
	}
}
