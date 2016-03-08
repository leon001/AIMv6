
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
	void (*send)(unsigned long, uint8_t);
	/*
	 * For Port I/O,
	 * drive->base = 0x1f0 if primary else 0x170
	 *
	 * For PCI,
	 * drive->base = BAR0 if primary else BAR2
	 */
	unsigned long base = drive->base;

	/*
	 * For Port I/O, send = outb
	 * For PCI, send = out8
	 */
	send = drive->bus->send;

	send(base + ATA_REG_DATA, tf->data);
	send(base + ATA_REG_ERROR, tf->error);
	send(base + ATA_REG_NSECT, tf->count);
	send(base + ATA_REG_LBAL, tf->lbal);
	send(base + ATA_REG_LBAM, tf->lbam);
	send(base + ATA_REG_LBAH, tf->lbah);
	send(base + ATA_REG_DEVSEL, tf->devsel);
	send(base + ATA_REG_CMD, tf->command);
}

void
ide_recv(struct ide_drive *drive, struct ata_tf *tf)
{
	drive->tf_recv(drive, tf);
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
