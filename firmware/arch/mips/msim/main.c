
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <libc/stddef.h>
#include <smp.h>
#include <drivers/serial/uart.h>
#include <drivers/block/hd.h>
#include <drivers/block/msim-ddisk.h>

#define FWSTACKSIZE	(1 << FWSTACKORDER)

unsigned char fwstack[NR_CPUS][FWSTACKSIZE];

/*
 * Function pointer declaration for MBR bootloader entry.
 * The bootloader entry should look like:
 *
 * void entry(msim_dd_read_sector_t read_sector, unsigned long disk_physaddr)
 *
 * See boot/arch/mips/msim/bootsect.c for demonstration (TBD)
 *
 * Definition for msim_dd_read_sector_t is in
 * drivers/block/msim-ddisk.h
 */
typedef void (*mbr_entry_t)(msim_dd_read_sector_t, unsigned long);

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
		 * argument storing the address of the "read disk sector"
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
		 *     "read disk sector" function, and the physical address
		 *     of the hard disk.
		 */
		if (mbr[SECTOR_SIZE - 2] == 0x55 &&
		    mbr[SECTOR_SIZE - 1] == 0xaa) {
			(*(mbr_entry_t)mbr)(msim_dd_read_sector,
			    MSIM_DISK_PHYSADDR);
		} else {
			uart_puts("Corrupted legacy boot sector?\n");
		}
	}
	for (;;)
		/* nothing */;
}

