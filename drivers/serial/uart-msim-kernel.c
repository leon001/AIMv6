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

#include <sys/types.h>
#include <aim/device.h>
#include <aim/initcalls.h>
#include <proc.h>
#include <vmm.h>
#include <errno.h>
#include <mach-conf.h>
#include <libc/string.h>

#define LP_DEVICE_NAME	"lpr"
#define KBD_DEVICE_NAME	"kbd"

/* Common code for open */
static int __open(dev_t devno, int mode, struct proc *p, const char *devname,
    addr_t paddr, bool kbd)
{
	struct chr_device *dev;
	kprintf("DEV: opening %s device\n", kbd ? "keyboard" : "printer");

	dev = (struct chr_device *)dev_from_id(devno);
	if (dev == NULL) {
		dev = kmalloc(sizeof(*dev), GFP_ZERO);
		if (dev == NULL)
			return -ENOMEM;
		initdev(dev, devname, devno);
		/* XXX for now we hardwire to memory bus */
		dev->bus = &early_memory_bus;
		dev->base = paddr;	/* assuming only one device */
		dev_add(dev);
		__uart_msim_init(kbd ? NULL : dev, kbd ? dev : NULL);
	}
	return 0;
}

/* Common code for close */
static int __close(dev_t devno, int mode, struct proc *p)
{
	/* Currently we do nothing */
	return 0;
}

static int __lpopen(dev_t devno, int mode, struct proc *p)
{
	return __open(devno, mode, p, LP_DEVICE_NAME, MSIM_LP_PHYSADDR, false);
}

static int __kbdopen(dev_t devno, int mode, struct proc *p)
{
	return __open(devno, mode, p, KBD_DEVICE_NAME, MSIM_KBD_PHYSADDR, true);
}

static struct chr_driver lpdrv = {
	.type = DRV_CHR,
	.open = __lpopen,
	.close = __close
};

static struct chr_driver kbddrv = {
	.type = DRV_CHR,
	.open = __kbdopen,
	.close = __close
};

static int __driver_init(void)
{
	register_driver(MSIM_LP_MAJOR, &lpdrv);
	register_driver(MSIM_KBD_MAJOR, &kbddrv);
	/* TODO: add interrupt handler */
	return 0;
}
INITCALL_DRIVER(__driver_init);
