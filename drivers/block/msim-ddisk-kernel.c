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
#include <aim/device.h>
#include <mach-conf.h>
#include <aim/initcalls.h>

static int __open(dev_t dev, int mode, struct proc *p)
{
	kprintf("opened\n");
	return 0;
}

static struct blk_driver drv = {
	.open = __open
};

static int __driver_init(void)
{
	register_driver(MSIM_DISK_MAJOR, &drv);
	return 0;
}
INITCALL_DRIVER(__driver_init);

