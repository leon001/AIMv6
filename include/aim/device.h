/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
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

#ifndef _DEVICE_H
#define _DEVICE_H

#include <sys/types.h>
#include <aim/sync.h>
#include <list.h>

/* forward */
struct bus_device;
struct proc;	/* include/proc.h */
struct buf;	/* include/buf.h */
struct uio;	/* fs/uio.h */

/* Drivers */
struct driver {
	int type;
#define DRV_CHR		1
#define DRV_BLK		2
	int (*open)(dev_t dev, int oflags, struct proc *p);
	int (*close)(dev_t dev, int oflags, struct proc *p);
};

struct chr_driver {
	struct driver;
	int (*read)(dev_t dev, struct uio *uio, int ioflags);
	int (*write)(dev_t dev, struct uio *uio, int ioflags);
};

struct blk_driver {
	struct driver;
	int (*strategy)(struct buf *bp);
};

struct net_driver {
	struct driver;
};

extern struct driver *devsw[];

void register_driver(unsigned int major, struct driver *drv);

/* Devices */
#define DEV_NAME_MAX	64
struct device {
	char name[DEV_NAME_MAX];

	dev_t devno;

	struct bus_device *bus;
	addr_t base;

	lock_t lock;
};

struct chr_device {
	struct device;

	/* Reserved for later use */
};

struct blk_device {
	struct device;

	struct list_head bufqueue;
	/* Reserved for later use */
};

struct net_device {
	struct device;

	/* Reserved for later use */
};

/* Bus subsystem details */

/*
 * Buses vary in address width, and may provide multiple data access width.
 * A single bus may have quite some read/write routines, and may even not come
 * in pairs.
 * Bus access may or may not encounter failures. In the latter case, access
 * routines simply return 0 to indicate a success.
 * TODO: explain why we fix buffer pointer as a uint64_t pointer.
 */
typedef int (*bus_read_fp)(struct bus_device * inst,
	addr_t addr, uint64_t *ptr);
typedef int (*bus_write_fp)(struct bus_device * inst,
	addr_t addr, uint64_t val);

/*
 * To get acess routines like listed above, a caller should ask for them.
 * Data width MUST be given. These routines may return now in and only in cases
 * that the underlying bus controller cannot handle the given data width.
 * A single bus cannot have multiple address widths, and the value is written
 * in struct bus_device.
 */

struct bus_device {
	struct device;
	int addr_width;
	bus_read_fp (*get_read_fp)(
		struct bus_device * inst, int data_width);
	bus_write_fp (*get_write_fp)(
		struct bus_device * inst, int data_width);
};

/* Managing devices */

struct device_index {
	int (*add)(struct device *dev);
	int (*remove)(struct device *dev);
	struct device *(*from_id)(dev_t devno);
	struct device *(*from_name)(char *name);
};

void set_device_index(struct device_index *index);

int dev_add(struct device *dev);
int dev_remove(struct device *dev);
struct device *dev_from_id(dev_t devno);
struct device *dev_from_name(char *name);
void initdev(struct device *dev, const char *devname, dev_t devno);

#endif /* _DEVICE_H */

