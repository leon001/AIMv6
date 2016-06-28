/* Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
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

/*
 * This is the code for kernel driver.
 * It is separated from msim-ddisk.c for clarity and is included as is in
 * msim-ddisk.c by #include.
 * The internal routines are already provided in msim-ddisk.c
 */

#include <sys/types.h>
#include <proc.h>
#include <buf.h>
#include <fs/bio.h>
#include <aim/device.h>
#include <mach-conf.h>
#include <aim/initcalls.h>
#include <drivers/io/io-mem.h>
#include <errno.h>
#include <panic.h>
#include <drivers/hd/hd.h>

static void __init(struct hd_device *hd)
{
	kprintf("DEBUG: initializing MSIM hard disk\n");
	__msim_dd_init(hd);
	kprintf("DEBUG: initialization done\n");
}

static int __open(dev_t dev, int mode, struct proc *p)
{
	struct hd_device *hdpart;
	struct hd_device *hd;

	kprintf("DEBUG: __open: %d, %d\n", major(dev), minor(dev));

	hdpart = (struct hd_device *)dev_from_id(dev);
	hd = (struct hd_device *)dev_from_id(hdbasedev(dev));

	/* Create a device for whole hard disk if the device structure does
	 * not exist. */
	if (hd == NULL) {
		hd = kmalloc(sizeof(*hd), 0);
		if (hd == NULL)
			return -ENOMEM;

		memset(hd, 0, sizeof(*hd));
		strlcpy(hd->name, "Md", DEV_NAME_MAX);
		hd->devno = hdbasedev(dev);
		/*
		 * XXX
		 * For now we hardwire the disk bus to memory bus.
		 */
		hd->bus = &early_memory_bus;
		hd->base = MSIM_DISK_PHYSADDR;
		list_init(&(hd->bufqueue));
		spinlock_init(&hd->lock);
		dev_add(hd);

		__init(hd);
	}

	/* Detect all partitions if we could not find the partition device
	 * structure. */
	if (hdpart == NULL) {
		detect_hd_partitions(hd);
	}
	return 0;
}

/*
 * The real place where struct buf's are turned into disk commands.
 * Assumes that the buf queue lock is held.
 */
static void __start(struct hd_device *dev)
{
	struct buf *bp;
	off_t blkno, partoff;
	int partno;

	assert(!list_empty(&dev->bufqueue));
	bp = list_first_entry(&dev->bufqueue, struct buf, ionode);
	assert(bp->nblksrem != 0);
	assert(bp->flags & (B_DIRTY | B_INVALID));
	assert(bp->flags & B_BUSY);
	partno = hdpartno(bp->devno);
	assert((partno == 0) || (dev->part[partno].len != 0));
	partoff = (partno == 0) ? 0 : dev->part[partno].offset;
	blkno = bp->blkno + bp->nblks - bp->nblksrem + partoff;

	if (bp->flags & B_DIRTY) {
		kprintf("DEBUG: writing to %d from %p\n", blkno, bp->data);
		__msim_dd_write_sector(dev, blkno, bp->data, false);
	} else if (bp->flags & B_INVALID) {
		kprintf("DEBUG: reading from %d to %p\n", blkno, bp->data);
		__msim_dd_read_sector(dev, blkno, bp->data, false);
	}
	/* TODO NEXT: deal with interrupt callbacks */
	panic("done\n");
}

static int __intr(void)
{
	/*
	 * Theoretically we need to enumerate all hard disks to check
	 * if an interrupt is asserted.  Here we assumes that we only
	 * have one device: rootdev.
	 */
	struct hd_device *hd;
	extern dev_t rootdev;	/* mach_init() or arch_init() */
	unsigned long flags;
	struct buf *bp;
	void *dst;

	hd = (struct hd_device *)dev_from_id(hdbasedev(rootdev));
	if (!__msim_dd_check_interrupt(hd)) {
		panic("entering interrupt handler without rootdev interrupt?\n");
		/* return 0; */
	}
	__msim_dd_ack_interrupt(hd);
	if (__msim_dd_check_error(hd)) {
		bp->errno = -EIO;
		bp->flags |= B_ERROR;
		kprintf("DEBUG: fail buf %p\n", bp);
		list_del(&(bp->ionode));
		biodone(bp);
		goto next;
	}

	spin_lock_irq_save(&hd->lock, flags);

	if (list_empty(&hd->bufqueue)) {
		kprintf("DEBUG: spurious interrupt?\n");
		return 0;
	}
	bp = list_first_entry(&hd->bufqueue, struct buf, ionode);
	assert(bp->flags & B_BUSY);
	assert(bp->flags & (B_INVALID | B_DIRTY));
	dst = bp->data + (bp->nblks - bp->nblksrem) * BLOCK_SIZE;
	if (!(bp->flags & B_DIRTY) && (bp->flags & B_INVALID))
		__msim_dd_fetch(hd, dst);
	bp->nblksrem--;
	kprintf("DEBUG: buf %p remain %d\n", bp, bp->nblksrem);

	if (bp->nblksrem == 0) {
		kprintf("DEBUG: done buf %p\n", bp);
		bp->flags &= ~(B_DIRTY | B_INVALID);
		biodone(bp);
		goto next;
	}

next:
	if (!list_empty(&hd->bufqueue))
		__start(hd);

	spin_unlock_irq_restore(&hd->lock, flags);
}

static int __strategy(struct buf *bp)
{
	struct hd_device *hd;
	unsigned long flags;

	assert(bp->flags & B_BUSY);
	if (!(bp->flags & (B_DIRTY | B_INVALID))) {
		kprintf("DEBUG: msim-ddisk: nothing to do\n");
		return 0;
	}
	kprintf("DEBUG: msim-ddisk: queuing buf %p with %d blocks at %p\n", bp, bp->nblks, bp->data);

	bp->nblksrem = bp->nblks;

	hd = (struct hd_device *)dev_from_id(hdbasedev(bp->devno));

	spin_lock_irq_save(&hd->lock, flags);
	if (list_empty(&hd->bufqueue)) {
		list_add_tail(&bp->ionode, &hd->bufqueue);
		__start(hd);
	} else {
		list_add_tail(&bp->ionode, &hd->bufqueue);
	}
	spin_unlock_irq_restore(&hd->lock, flags);
	return 0;
}

static struct blk_driver drv = {
	.open = __open,
	.strategy = __strategy
};

static int __driver_init(void)
{
	register_driver(MSIM_DISK_MAJOR, &drv);
	return 0;
}
INITCALL_DRIVER(__driver_init);

