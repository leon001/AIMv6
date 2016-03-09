
/*
 * This file reads SATA disks in IDE mode.
 * The following functions will decompose into the following primitives:
 * ide_recv(DRV, TASKFILE)
 * ide_send(DRV, TASKFILE)
 *
 * For device control and alternate status, send to
 * ide_control(DRV, DATA)
 * ide_altstat(DRV)
 *
 * ide_recv() and ide_send() decide how to actually read/write into
 * ports/memory-mapped registers.
 */

void
ide_send(struct ide_drive *drive, struct ata_tf *tf)
{
	/* Pass the message to send (task file here) as well as drive
	 * information (base address, device number, etc.) to bus driver. */
	drive->bus->send(drive, tf);
}

void
ide_recv(struct ide_drive *drive, struct ata_tf *tf)
{
	drive->bus->recv(drive, tf);
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
	struct ata_tf tf;

	tf.data = 0;
	tf.error = 0;
	tf.count = count;
	tf.lbal = sector & 0xff;
	tf.lbam = (sector >> 8) & 0xff;
	tf.lbah = (sector >> 16) & 0xff;
	tf.devsel = ATA_LBA
	    | (drive->slave ? ATA_DEV1 : 0)
	    | (sector >> 24) & 0xf;
	tf.command = ATA_CMD_PIO_READ;

	ide_send(drive, &tf);
}

/* TODO: do we need length check? */
void
ide_fetch_sector(struct ide_drive *drive, unsigned char *buf)
{
	size_t pos = 0;
	struct ata_tf tf;

	for (ide_recv(drive, &tf);
	    tf.status & ATA_DRQ;
	    ide_recv(drive, &tf)) {
		buf[pos++] = tf->data;
	}
}
