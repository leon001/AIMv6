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
	__msim_dd_init(hd->base);
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

/* The real place where struct buf's are turned into disk commands */
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

	if (bp->flags & B_DIRTY)
		__msim_dd_write_sector(dev->base, blkno, bp->data, false);
	else if (bp->flags & B_INVALID)
		__msim_dd_read_sector(dev->base, blkno, bp->data, false);
	/* TODO NEXT: deal with interrupt callbacks */
	panic("done");
}

static int __strategy(struct buf *bp)
{
	struct hd_device *hd;

	assert(bp->flags & B_BUSY);
	if (!(bp->flags & (B_DIRTY | B_INVALID)))
		return 0;

	hd = (struct hd_device *)dev_from_id(hdbasedev(bp->devno));

	spin_lock(&hd->lock);
	if (list_empty(&hd->bufqueue)) {
		list_add_tail(&bp->ionode, &hd->bufqueue);
		__start(hd);
	} else {
		list_add_tail(&bp->ionode, &hd->bufqueue);
	}
	spin_unlock(&hd->lock);
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

