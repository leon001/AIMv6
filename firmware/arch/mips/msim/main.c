
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <libc/stddef.h>
#include <libc/string.h>
#include <smp.h>
#include <drivers/serial/uart.h>
#include <drivers/block/hd.h>
#include <drivers/block/msim-ddisk.h>

#define FWSTACKSIZE	(1 << FWSTACKORDER)

unsigned char fwstack[NR_CPUS][FWSTACKSIZE];

typedef void (*readdisk_t)(size_t, size_t, void *, size_t);
typedef void (*mbr_entry_t)(readdisk_t, uintptr_t);

void fwpanic(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	uart_vprintf(fmt, ap);
	for (;;)
		/* nothing */;
	va_end(ap);
}

void readdisk(size_t sector, size_t offset, void *buf, size_t len)
{
	unsigned char sector_buf[SECTOR_SIZE];
	size_t l = 0;

	sector += offset / SECTOR_SIZE;
	offset %= SECTOR_SIZE;

	for (; len > 0; len -= l) {
		l = MIN2(len, SECTOR_SIZE - offset);
		if (msim_dd_read_sector(MSIM_DISK_PHYSADDR,
		    sector, sector_buf, true) < 0)
			fwpanic("read disk error");
		memcpy(buf, &sector_buf[offset], l);
		offset = 0;
		buf += l;
		++sector;
	}
}

void main(void)
{
	char mbr[SECTOR_SIZE];
	uart_puts("Hello world!\n");
	msim_dd_init(MSIM_DISK_PHYSADDR);
	if (msim_dd_read_sector(MSIM_DISK_PHYSADDR, 0, mbr, true) == 0) {
		/*
		 * DESIGN NOTE:
		 * MIPS instructions are loosely-encoded, so it's usually
		 * not possible to embed the disk driver into MBR, as most
		 * legacy IBM PC-compatible computers (which usually have
		 * x86 cores) do.
		 *
		 * Unfortunately, there's no suitable bootloader standard
		 * for MIPS processors other than UEFI.  The utmost advantage
		 * for UEFI is CPU independence.  However, UEFI is too
		 * complicated as it must support GUID Partition Table (GPT),
		 * and even calls for minimal filesystem support.
		 *
		 * MSIM doesn't ship a firmware; you have to design it
		 * yourself.  And since we're mainly dealing with kernel
		 * design rather than firmware here, we choose an simple
		 * yet ugly design: to couple firmware and MBR together.
		 * The firmware jumps to MBR bootloader entry with an
		 * argument storing the address of the "read disk"
		 * function.  This design sticks firmware, disk hardware,
		 * and MBR together, and is thus a *BAD* design, however
		 * we don't want to waste too much effort here.
		 *
		 * Those interested in UEFI can contribute to our repo.
		 * Especially for MIPS developers, if you do work out a
		 * UEFI firmware, tell Loongson and they will be probably
		 * very grateful.
		 */

		/*
		 * This statement jumps into a function whose address is the
		 * same as buffer variable "mbr", where we just put the first
		 * disk sector.  Since the first 446 bytes of MBR (and hence
		 * the buffer "mbr") contains the bootloader code, jumping
		 * there transfers control to the MBR bootloader.
		 *
		 * For those unfamiliar with function pointers:
		 * This statement does the following:
		 * (1) obtain the buffer address ("mbr")
		 * (2) view it as a function entry with type mbr_entry_t
		 *     (see the "typedef" statement above for definition)
		 * (3) execute the function there with two arguments: the
		 *     "read disk" function, and the address of MBR, i.e.
		 *     the address of bootloader entry itself.  The reason
		 *     is discussed in boot/arch/mips/msim/bootsect.c.
		 */
		(*(mbr_entry_t)mbr)(readdisk, mbr);
	}
	for (;;)
		/* nothing */;
}

