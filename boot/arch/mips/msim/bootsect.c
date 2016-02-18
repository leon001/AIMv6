
#include <elf.h>
#include <bootsect.h>
#include <libc/stddef.h>

/* Sync to firmware/arch/mips/msim/main.c */
typedef void (*readdisk_t)(size_t, size_t, void *, size_t);
typedef void (*entry_t)(void);

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
void boot(readdisk_t readdisk, uintptr_t mbr_addr)
{
	struct elf32hdr eh;
	struct elf32_phdr ph;
	Elf32_Half i;
	entry_t entry;
	Elf32_Off pos = 0;

#if 1
	struct mbr *mbr = (struct mbr *)mbr_addr;
	uint32_t lba = mbr->part_entry[1].first_sector_lba;
#else
	unsigned long addr = (unsigned long)mbr_addr + PART0_ENTRY_OFFSET
	    + sizeof(struct mbr_part_entry)
	    + MEMBER_OFFSET(struct mbr_part_entry, first_sector_lba);
	uint32_t lba = *(uint16_t *)(addr + 2);
	lba = (lba << 16) + *(uint16_t *)addr;
#endif
	uintptr_t seg;

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

	entry = (entry_t)(eh.e_entry);
	(*entry)();
}

