/* Copyright (C) 2016 Xiaofei Bai <xffbai@gmail.com>
 * Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
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

/* A hack for disabling the preprocessor to replace $ARCH automatically */
#undef i386

OUTPUT_FORMAT(elf32-i386)
ENTRY(master_entry)

SECTIONS
{
	/DISCARD/ : {}

	/* Note that we should probably reserve further more space for
	 * bootloader use.  See boot/arch/i386/bootmain.c for such
	 * demonstration. */

	/*
	 * IMPORTANT:
	 * Strangely, sections such as .low_text are not put into a
	 * loadable segment.
	 *
	 * As such, I'm unable to put the virtual addresses into high
	 * address space (>KERN_BASE), since doing that will make
	 * the entry reside in high address space as well.
	 *
	 * Another consequence is that get_addr_space() have to return
	 * 1 all the time to make __get_kputchar() find the correct
	 * address.
	 */
	. = ALIGN(256K);

	/* . += KERN_BASE; */

	_text_begin = .;
	.text : {
		*(.text*);
		_text_end = .;
	}
	
	.rodata : {
		*(.rodata*);
		_rodata_end = .;
	}
	
	.bss : {
		_bss_begin = .;
		*(.bss*);
		_bss_end = .;
	}

	.data : {
		*(.data*);
	}

	.got : {
		*(.got*);
		*(.igot*);
	}

	/* probably redundant */
	.plt : {
		*(.plt*);
	}

	. = ALIGN(16K);

	.boot : {
		PROVIDE(boot_pgdir = .);
		. += 16K; /* early pgdir */
		. += 4K; /* early stack */
		PROVIDE(boot_stack = .);
	}
}
