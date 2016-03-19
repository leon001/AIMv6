/*
 * LICENSE NOTES:
 * Since most i386 code comes from MIT xv6, I wonder how we should put the
 * license information...
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <asm.h>
#include <elf.h>
#include <sys/types.h>
#include <drivers/ata/ata.h>

/* ELF buffer address for storing read headers */
#define ELF_BUFFER	0x10000L

#define IDE_PORTBASE	0x1f0

/* TODO: Should be merged into config.h? */
#define SECTOR_SIZE	512
#define PGSIZE		4096

/* Convenient and readable macros instead of raw inb() and outb() */
#define IDE_PIO_READ(reg)		inb(IDE_PORTBASE + (reg))
#define IDE_PIO_WRITE(reg, data)	outb(IDE_PORTBASE + (reg), data)
#define IDE_PIO_FETCH(dst)	\
	insl(IDE_PORTBASE + ATA_REG_DATA, dst, SECTOR_SIZE / 4)

static inline void
waitdisk(void)
{
	while ((IDE_PIO_READ(ATA_REG_STATUS) & (ATA_BUSY | ATA_DRDY)) !=
	    ATA_DRDY)
		/* nothing */;
}

static void
readsect(void *dst, size_t sector)
{
	/* For bootloaders, the code should be as compact as possible. */
	waitdisk();

	/* A typical ATA command sequence.  Pretty self-explanatory. */
	IDE_PIO_WRITE(ATA_REG_NSECT, 1);
	IDE_PIO_WRITE(ATA_REG_LBAL, sector & 0xff);
	IDE_PIO_WRITE(ATA_REG_LBAM, (sector >> 8) & 0xff);
	IDE_PIO_WRITE(ATA_REG_LBAH, (sector >> 16) & 0xff);
	IDE_PIO_WRITE(ATA_REG_DEVSEL,
	    ((sector >> 24) & 0xff) | (ATA_DEVICE_OBS | ATA_LBA));
	IDE_PIO_WRITE(ATA_REG_CMD, ATA_CMD_PIO_READ);

	waitdisk();

	IDE_PIO_FETCH(dst);
}

static void
readseg(unsigned char *pa, size_t count, size_t offset)
{
	unsigned char *epa = pa + count;
	unsigned int sector = (offset / SECTOR_SIZE) + 1;

	pa -= offset % SECTOR_SIZE;

	for (; pa < epa; pa += SECTOR_SIZE, ++sector)
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
	readseg((unsigned char *)elf, PGSIZE, 0);

	/*
	 * Traverse each program header and load every segment.
	 * I wonder why xv6 reads every segment regardless of flags.
	 */
	ph = (struct elf32_phdr *)(ELF_BUFFER + elf->e_phoff);
	eph = ph + elf->e_phnum;

	for (; ph < eph; ++ph) {
		pa = (unsigned char *)(ph->p_paddr);
		readseg((unsigned char *)pa, ph->p_filesz, ph->p_offset);
		if (ph->p_memsz > ph->p_filesz)
			/*
			 * We don't use memset() here because we don't want
			 * to add libc dependency into our bootloader.
			 */
			stosb(pa + ph->p_filesz,
			    0,
			    ph->p_memsz - ph->p_filesz);
	}

	/* Jump to ELF entry... */
	entry = (void (*)(void))(elf->e_entry);
	entry();
}
