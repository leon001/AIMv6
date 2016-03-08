
/*
 * This file reads SATA disks in IDE mode.
 * The following functions will decompose into the following primitives:
 * ide_recv(DRV, REG)
 * ide_send(DRV, REG, DATA)
 *
 * For device control and alternate status, send to
 * ide_control(DRV, DATA)
 * ide_altstat(DRV)
 *
 * ide_recv() and ide_send() decide how to actually read/write into
 * ports/memory-mapped registers.
 */

void
ide_send(struct ide_drive *drive, uint8_t reg, uint8_t data)
{
}

void
ide_disable_interrupt(struct ide_drive *drive)
{
	ide_control(drive, ATA_NIEN);
}

void
ide_enable_interrupt(struct ide_drive *drive)
{
	ide_control(drive, 0);
}

/*
 * Issue a PIO READ command to IDE drive for at most 256 sectors in 28-bit
 * LBA mode
 */
void
ide_do_read_pio_256_lba28(struct ide_drive *drive, size_t sector, uint8_t count)
{
	ide_send(drive, ATA_REG_NSECT, count);
	ide_send(drive, ATA_REG_LBAL, (uint8_t)(sector));
	ide_send(drive, ATA_REG_LBAM, (uint8_t)(sector >> 8));
	ide_send(drive, ATA_REG_LBAH, (uint8_t)(sector >> 16));
	ide_send(drive, ATA_REG_DEVSEL, (uint8_t)(ATA_LBA
	    | (drive->slave ? ATA_DEV1 : 0)
	    | ((sector >> 24) & 0xf)));
	ide_send(drive, ATA_REG_CMD, ATA_CMD_PIO_READ);
}

/* TODO: do we need length check? */
void
ide_fetch_data(struct ide_drive *drive, unsigned char *buf)
{
	size_t pos = 0;

	while (ide_recv(drive, ATA_REG_STATUS) & ATA_DRQ)
		/* TODO: do we need to repeat this statement? we usually
		 * read them in pairs. */
		buf[pos++] = ide_recv(drive, ATA_REG_DATA);
}
