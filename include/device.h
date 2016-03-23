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

#include <file.h>

struct device {
	const char * name;
	int id_major, id_minor;

	//struct bus_type * bus;
	//struct device * parent;

	struct file_ops file_ops;

	struct device_driver * driver;
};

struct chr_device {
	struct device;

	/* Reserved for later use */
};

struct blk_device {
	struct device;

	struct blk_ops blk_ops;
};

struct net_device {
	struct device;

	/* Reserved for later use */
};

/* Bus subsystem details */

/* forward */
struct bus_device;

/*
 * Buses vary in address width, and may provide multiple data access width.
 * A single bus may have quite some read/write routines, and may even not come
 * in pairs.
 * Bus access may or may not encounter failures. In the latter case, access
 * routines simply return 0 to indicate a success.
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
struct bus_ops {
	bus_read_fp * (*bus_get_read_fp)(
		struct bus_device * inst, int data_width);
	bus_write_fp * (*bus_get_write_fp)(
		struct bus_device * inst, int data_width);
};

struct bus_device {
	struct device;
	int addr_width;
	struct bus_ops bus_ops;
};

#endif /* _DEVICE_H */

