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
}

struct chr_device {
	struct device;

	/* Reserved for later use */
}

struct blk_device {
	struct device;

	struct blk_ops blk_ops;
}

struct net_device {
	struct device;

	/* Reserved for later use */
}

#endif /* _DEVICE_H */

