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
#include <drivers/tty/tty.h>
#include <proc.h>
#include <vmm.h>
#include <errno.h>
#include <mach-conf.h>	/* TTY_MAJOR */
#include <libc/string.h>

#define DEVICE_NAME	"tty"

/*
 * /dev/tty is the *DEFAULT* terminal device.
 *
 * Each platform has its own "default" terminal device.  However, how to
 * specify the composition of that default terminal, remains to be debated.
 * 1. Hardwire it in kernel codebase.  Changing the device requires
 *    recompiling the kernel.
 * 2. Store the device numbers in a file.  Probably unsafe.
 * 3. ...?
 *
 * A process can change its underlying terminal device via ioctl().
 */

extern int setup_default_tty(struct tty_device *, int, struct proc *);

static int __open(dev_t devno, int mode, struct proc *p)
{
	struct tty_device *tty;
	int err;

	tty = (struct tty_device *)dev_from_id(devno);
	if (tty == NULL) {
		tty = kmalloc(sizeof(*tty), GFP_ZERO);
		if (tty == NULL)
			return -ENOMEM;
		initdev(tty, DEVICE_NAME, devno);
		err = setup_default_tty(tty, mode, p);
		if (err) {
			kfree(tty);
			return err;
		}
		dev_add(tty);
	}

	/* TODO FUTURE: check and fill sessions */
	return 0;
}

static int __close(dev_t dev, int oflags, struct proc *p)
{
	/* Currently we do nothing */
	return 0;
}

static struct chr_driver ttydrv = {
	.type = DRV_CHR,
	.open = __open,
	.close = __close
};

static int __driver_init(void)
{
	register_driver(TTY_MAJOR, &ttydrv);
	return 0;
}
INITCALL_DRIVER(__driver_init);

