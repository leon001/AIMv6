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

#include <elf.h>
#include <bootsect.h>
#include <libc/stddef.h>

/*
 * The first 446 bytes contains this piece of code.
 *
 * The reason why we include the address of this entry as an argument
 * is that the compiler/assembler usually doesn't know where our code
 * is located when computing addresses.
 * In the case here, the partition entries are at address (boot + 446).
 * However, as the compiler and assembler doesn't know where the boot()
 * function is, and therefore they are unable to find the exact address.
 *
 * Position-Independent Code (PIC) resolve this issue.  In fact, PIC
 * is a fundamental technique in dynamic linking.  The most important
 * consequence of introducing PIC is that the applications no longer
 * need to bring the code of C library functions by themselves.
 * However, using PIC in firmware, bootloader and kernel is rather tedious,
 * so we are not going to do it here.
 */
void boot(generic_funcptr readdisk, uintptr_t mbr_addr)
{
	struct elf32hdr eh;
	struct elf32_phdr ph;
	Elf32_Half i;
	generic_funcptr entry;
	Elf32_Off pos = 0;

	struct mbr *mbr = (struct mbr *)mbr_addr;
	uint32_t lba = mbr->part_entry[1].first_sector_lba;
	uintptr_t seg;

	/*
	 * Basically, we're reading a statically-linked ELF file.
	 * The logic is very simple: we locate every loadable segment
	 * in the ELF file, putting the content in appropriate memory
	 * address.  After everything is loaded into the memory, jump
	 * to the entry address and continue execution.
	 *
	 * Ideally, we want to clean ".bss" section prior to jumping
	 * into the entry point.  However, the space in MBR is very
	 * limited, so we'll leave the job to the kernel itself.
	 */

	/* Read the ELF header first */
	readdisk(lba, pos, &eh, sizeof(eh));

	pos = eh.e_phoff;
	for (i = 0; i < eh.e_phnum; ++i) {
		readdisk(lba, pos, &ph, sizeof(ph));
		if (ph.p_type == PT_LOAD) {
			seg = (void *)ph.p_vaddr;
			readdisk(lba, ph.p_offset, seg, ph.p_filesz);
		}
		pos += eh.e_phentsize;
	}

	entry = (generic_funcptr)(eh.e_entry);
	(*entry)();
}

