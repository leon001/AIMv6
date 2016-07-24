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
	int class;
#define DEVCLASS_CHR	1
#define DEVCLASS_BLK	2
#define DEVCLASS_NET	3
#define DEVCLASS_BUS	4
	int type;
#define DRVTYPE_NORMAL	0
#define DRVTYPE_TTY	1
	int (*open)(dev_t dev, int oflags, struct proc *p);
	int (*close)(dev_t dev, int oflags, struct proc *p);
};

struct chr_driver {
	struct driver;
	int (*read)(dev_t dev, struct uio *uio, int ioflags);
	int (*write)(dev_t dev, struct uio *uio, int ioflags);

	/*
	 * These methods are intended for internal use.  They should not
	 * be invoked outside the driver framework.
	 */
	int (*getc)(dev_t dev);
	int (*putc)(dev_t dev, int c);
};

struct blk_driver {
	struct driver;
	int (*strategy)(struct buf *bp);
};

struct net_driver {
	struct driver;
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

struct bus_driver {
	struct driver;
	bus_read_fp (*get_read_fp)(
		struct bus_device * inst, int data_width);
	bus_write_fp (*get_write_fp)(
		struct bus_device * inst, int data_width);
};

extern struct driver *devsw[];

void register_driver(unsigned int major, struct driver *drv);

/* Devices */
#define DEV_NAME_MAX		64
#define DEV_REG_MAX		10
#define DEV_INTRCELL_MAX	2
struct device {
	char name[DEV_NAME_MAX];

	int class;
	dev_t devno;

	struct bus_device *bus;
	union {
		/* Some devices (e.g. PC IDE hard disk) have multiple
		 * register spaces with different base addresses. */
		addr_t bases[DEV_REG_MAX];
		/* But most devices only have one.  In that case, we
		 * could as well use this member instead. */
		addr_t base;
	};

	union {
		struct driver driver;
		struct chr_driver chr_driver;
		struct blk_driver blk_driver;
		struct net_driver net_driver;
		struct bus_driver bus_driver;
	};

	lock_t lock;

	/* Subsystem-specific data */
	void *subsys;
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

struct bus_device {
	struct device;
	int addr_width;
};

/* Managing devices */

struct device_index {
	int (*add)(struct device *dev);
	int (*remove)(struct device *dev);
	struct device *(*next)(struct device *dev, void **savep);
	struct device *(*from_id)(dev_t devno);
	struct device *(*from_name)(char *name);
};

void set_device_index(struct device_index *index);

int dev_add(struct device *dev);
int dev_remove(struct device *dev);
/* [TODO] should require the device list to be locked in order to avoid race
 * condition between dev_next and dev_add. */
struct device *dev_next(struct device *dev, void **savep);
struct device *dev_from_id(dev_t devno);
struct device *dev_from_name(char *name);
void initdev(struct device *dev, const char *devname, dev_t devno);

/*
 * Device tree structure
 *
 * This structure is a subset of Device Tree Specification.  Here I'm
 * assuming that:
 * 1. Only one integer is needed to describe an address (#address-cells == 1).
 * 2. Each device can send interrupt to at most one device.
 *
 * The root device tree node could only be "memory" or "portio".
 *
 * Interrupt nexus is not supported.
 */
struct devtree_entry {
	/* device name */
	char	name[DEV_NAME_MAX];
	/* device model, should match the ones provided in drivers */
	char	model[DEV_NAME_MAX];
	/* parent device (usually a bus) name */
	char	parent[DEV_NAME_MAX];
	/* number of register spaces on the parent bus */
	int	nregs;
	/* list of register space bases */
	addr_t	regs[DEV_REG_MAX];
	/* interrupt working mode */
	enum {
		DEVTREE_IM_NONE,	/* nothing to do with interrupt */
		DEVTREE_IM_GEN,		/* generates interrupt */
		DEVTREE_IM_CTRL,	/* is an interrupt controller */
	} intr_mode;
	/* the destination where this device will send interrupt, or "cpu" */
	char	intr_parent[DEV_NAME_MAX];
	/*
	 * The following are only valid if the device is an interrupt
	 * generator.
	 */
	/* number of cells/parameters to fully describe an interrupt, or
	 * a special negative value indicating how the interrupt descriptors
	 * are decided */
	int	nintrcells;
#define DEVTREE_INTR_AUTO	(-1)	/* own driver will decide */
	/* the descriptor of interrupt it generates, if @nintrcells > 0 */
	int	intr[DEV_INTRCELL_MAX];
};
extern struct devtree_entry devtree[];
extern int ndevtree_entries;	/* # of dev tree entries */

#endif /* _DEVICE_H */

