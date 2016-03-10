/*
 * LICENSE NOTES:
 * Since most i386 code comes from MIT xv6, I wonder how we should put the
 * license information...
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <elf.h>

/* ELF buffer address for storing read headers */
#define ELF_BUFFER	0x10000

void
bootmain(void)
{
	struct elf32hdr *elf = (struct elf32hdr *)ELF_BUFFER;
	struct elf32_phdr *ph, *eph;
	void (*entry)(void);
	unsigned char *pa;

	/* Read a reasonably sized block to fetch all headers we need */
	readseg(elf, PGSIZE, 0);

	/*
	 * Traverse each program header and load every segment.
	 * I wonder why xv6 reads every segment regardless of flags.
	 */
	ph = (struct elf32_phdr *)(ELF_BUFFER + elf->e_phoff);
	eph = ph + elf->phnum;

	for (; ph < eph; ++ph) {
		pa = (unsigned char *)(ph->p_paddr);
		readseg(pa, ph->p_filesz, ph->p_offset);
		if (ph->memsz > ph->filesz)
			memset(pa + ph->filesz, 0, ph->memsz - ph->filesz);
	}
}
