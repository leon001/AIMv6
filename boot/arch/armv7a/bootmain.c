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

/* from kernel */
#include <sys/types.h>
#include <elf.h>
#include <bootsect.h>
/* from libc */
#include <libc/stddef.h>

#define FW_BASE		0x1FF00000
#define MBR_ADDR	0x1FE00000

/*
 * The first 446 bytes contains this piece of code.
 *
 * For this MBR bootloader to work, 2 addresses need to be known at runtime:
 * 1. Address where MBR is loaded.
 * 2. Address of the firmware vector.
 *
 * To get the address of the running code, one often compiles his program into
 * Position-Independent Code (PIC), read the program counter, and (perhaps) do
 * a little bit of arithmatic operations.
 *
 * Or, one can set an agreement with his previous loader and coming callees, to
 * use a fixed address. This approach is widely used on IA32-based PCs. An
 * example is also available on ARMv7A platform.
 *
 * If the previous loader is identical to the caller, one can also pass
 * addresses as arguements, for which an example is present on mips platform.
 */

typedef void (*entry_t)(void);

void (*uart_puts)(const char *) = (void *)(FW_BASE + 0x04);
void (*readdisk)(size_t, size_t, void *, size_t) = (void *)(FW_BASE + 0x08);

__attribute__ ((noreturn))
int boot_main(void)
{
	struct elf32hdr eh;
	struct elf32_phdr ph;
	Elf32_Half i;
	entry_t entry;
	Elf32_Off pos = 0;

	uart_puts("BL: Hello!\r\n");

	struct mbr *mbr = (struct mbr *)MBR_ADDR;
	uint32_t lba;
	uintptr_t seg;

	uint8_t *src = (uint8_t *)&(mbr->part_entry[1].first_sector_lba);
	uint8_t *des = (uint8_t *)&lba;
	for (int i = 0; i < sizeof(lba); ++i) {
		des[i] = src[i];
	}

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
			seg = (void *)ph.p_paddr;
			readdisk(lba, ph.p_offset, seg, ph.p_filesz);
		}
		pos += eh.e_phentsize;
	}

	entry = (entry_t)(eh.e_entry);
	uart_puts("BL: Kernel loaded.\r\n");
	(*entry)();

//spin:
	while (1);
}
