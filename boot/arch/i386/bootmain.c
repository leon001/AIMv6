/*
 * LICENSE NOTES:
 * Since most i386 code comes from MIT xv6, I wonder how we should put the
 * license information...
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <elf.h>
#include <sys/types.h>
#include <drivers/ata/ata.h>

/* ELF buffer address for storing read headers */
#define ELF_BUFFER	0x10000

#define IDE_PORTBASE	0x1f0

/* TODO: Should be merged into config.h? */
#define SECTOR_SIZE	512

#define IDE_READ(reg, data)	inb(IDE_PORTBASE + (reg))
#define IDE_WRITE(reg, data)	outb(IDE_PORTBASE + (reg), data)
#define IDE_FETCH(dst)	\
	insl(IDE_PORTBASE + ATA_REG_DATA, dst, SECTOR_SIZE / 4)

static void
waitdisk(void)
{
	while ((IDE_READ(ATA_REG_STATUS) & (ATA_BUSY | ATA_DRDY)) != ATA_DRDY)
		/* nothing */;
}

static void
readsect(void *dst, size_t sector)
{
	/* For bootloaders, the code should be as compact as possible. */
	waitdisk();

	IDE_WRITE(ATA_REG_NSECT, 1);
	IDE_WRITE(ATA_REG_LBAL, sector & 0xff);
	IDE_WRITE(ATA_REG_LBAM, (sector >> 8) & 0xff);
	IDE_WRITE(ATA_REG_LBAH, (sector >> 16) & 0xff);
	IDE_WRITE(ATA_REG_DEVSEL,
	    ((sector >> 24) & 0xff) | (ATA_DEVICE_OBS | ATA_LBA));
	IDE_WRITE(ATA_REG_CMD, ATA_CMD_PIO_READ);

	waitdisk();

	IDE_FETCH(dst);
}

static void
readseg(unsigned char *pa, size_t count, size_t offset)
{
	unsigned char *epa = pa + count;
	unsigned int sector = (offset / SECTOR_SIZE) + 1;

	pa -= offset % SECTSIZE;

	for (; pa < epa; pa += SECT_SIZE, ++sector)
		readsect(pa, sector);
}

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

	entry = (void (*)(void))(elf->entry);
	entry();
}
