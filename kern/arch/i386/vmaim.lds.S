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

#include <mmu.h>
#include <asm-generic/vmaim.lds.h>

/* A hack for disabling the preprocessor to replace $ARCH automatically */
#undef i386

OUTPUT_ARCH(i386)
ENTRY(master_entry)

MEMORY
{
	PMEM (WX) :
		ORIGIN = KERN_START,
		LENGTH = MEM_SIZE - 1M - KERN_START
	VMEM (WX) :
		ORIGIN = KERN_BASE,
		LENGTH = MEM_SIZE - 1M
}

/*
 * If we do not manually specify how to arrange the sections into
 * segments, ld would try to do the job by its own.  A bad consequence
 * is that the virtual address (high address) and the physical address
 * (low address) usually does not match, which breaks the equation
 *
 * KVA = PA - KERN_START + KERN_BASE
 *
 * For the same reason, we have to manually specify the alignment of
 * each section, which should match the section headers displayed by
 * readelf(1).
 *
 * Double check the result of readelf(1), and make sure that the
 * equation above holds.
 */

PHDRS
{
	text_low PT_LOAD;
	text PT_LOAD;
	rodata PT_LOAD;
	data PT_LOAD;
}

SECTIONS
{
	/DISCARD/ : {
		/* This section wreaks havoc when arranging section
		 * .text.low and .text.  As this garbage section appears
		 * before .text.low, it essentially forces .text.low to
		 * overlap section .text.
		 *
		 * One can verify by removing this section from here and
		 * read the output of ld -M.
		 *
		 * Blacklisting this section results in a warning for
		 * --build-id option.  In most cases this doesn't matter,
		 * but I really hate warnings.  So one should add another
		 * linker flag: --build-id=none
		 */
		*(.note.gnu.build-id)
		/* .eh_frame and .eh_frame_hdr are also garbage sections
		 * messing up program segments.
		 */
		*(.eh_frame*)
	}

	.text.low KERN_START : {
		_text_low_begin = .;
		*(.text.low)
		_text_low_end = .;
	} >PMEM AT>PMEM :text_low

	.text __postmap_addr(_text_low_end) : {
		_text_begin = .;
		*(.text*)
		_text_end = .;
	} >VMEM AT>PMEM :text

	.got : ALIGN(4) {
		*(.got*)
		*(.igot*)
	} >VMEM AT>PMEM :rodata

	.rodata : ALIGN(4) {
		_rodata_begin = .;
		*(.rodata*)
		EARLY_INIT(STRUCT_ALIGNMENT)
		NORM_INIT(STRUCT_ALIGNMENT)
		LATE_INIT(STRUCT_ALIGNMENT)
		_rodata_end = .;
	} >VMEM AT>PMEM :rodata

	.data : ALIGN(32) {
		_data_begin = .;
		*(.data*)
		_data_end = .;
	} >VMEM AT>PMEM :data

	.bss : ALIGN(32) {
		_bss_begin = .;
		*(.bss)
		_bss_end = .;
	} >VMEM AT>PMEM :NONE

	.boot : {
		. = ALIGN(PAGE_SIZE);
		PROVIDE(boot_page_index = .);
		. += 16K; /* early pgdir */
		. += 4K; /* early stack */
		PROVIDE(boot_stack = .);
	} >VMEM AT>PMEM :NONE

	PROVIDE(_kern_end = ALIGN(PAGE_SIZE));
}


