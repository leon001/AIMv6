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
#include <aim/sync.h>
#include <aim/device.h>
#include <aim/initcalls.h>
#include <proc.h>
#include <vmm.h>
#include <errno.h>
#include <mach-conf.h>
#include <libc/string.h>
#include <trap.h>
#include <sched.h>
#include <fs/uio.h>
#include <asm-generic/funcs.h>

#define LP_DEVICE_NAME	"lpr"
#define KBD_DEVICE_NAME	"kbd"

static struct {
	char buf[BUFSIZ];
	int head;
	int tail;
	lock_t lock;
} cbuf = { {0}, 0, 0 };

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

static int __kbdintr(void)
{
	struct chr_device *kbd;
	unsigned char c;
	unsigned long flags;

	/* XXX I'm assuming there's only one keyboard */
	kbd = (struct chr_device *)dev_from_id(makedev(MSIM_KBD_MAJOR, 0));
	assert(kbd != NULL);
	c = __uart_msim_getchar(kbd);
	spin_lock_irq_save(&cbuf.lock, flags);
	if (cbuf.head == (cbuf.tail + 1) % BUFSIZ) {
		spin_unlock_irq_restore(&cbuf.lock, flags);
		return 0;	/* discard keyboard input */
	}
	cbuf.buf[cbuf.tail++] = c;
	cbuf.tail %= BUFSIZ;
	wakeup(&cbuf);
	spin_unlock_irq_restore(&cbuf.lock, flags);
	return 0;
}

static int __kbdgetc(dev_t devno)
{
	int c;
	unsigned long flags;

	/* XXX I'm assuming there's only one keyboard */
	spin_lock_irq_save(&cbuf.lock, flags);
	while (cbuf.head == cbuf.tail)
		sleep_with_lock(&cbuf, &cbuf.lock);
	c = cbuf.buf[cbuf.head++];
	cbuf.head %= BUFSIZ;
	spin_unlock_irq_restore(&cbuf.lock, flags);
	return c;
}

static int __lpputc(dev_t devno, int c)
{
	struct chr_device *lp;
	lp = (struct chr_device *)dev_from_id(devno);
	assert(lp != NULL);
	return __uart_msim_putchar(lp, c);
}

static int __lpwrite(dev_t devno, struct uio *uio, int ioflags)
{
	struct chr_device *lp;
	char buf[BUFSIZ];
	size_t len;
	int err, i;

	lp = (struct chr_device *)dev_from_id(devno);
	assert(lp != NULL);
	while (uio->resid > 0) {
		len = min2(uio->resid, BUFSIZ);
		err = uiomove(buf, len, uio);
		if (err)
			return err;
		for (i = 0; i < len; ++i)
			__uart_msim_putchar(lp, buf[i]);
	}

	return 0;
}

static struct chr_driver lpdrv = {
	.class = DEVCLASS_CHR,
	.open = __lpopen,
	.close = __close,
	.read = NOTSUP,
	.write = __lpwrite,
	.getc = NOTSUP,
	.putc = __lpputc	/* NYI */
};

static struct chr_driver kbddrv = {
	.class = DEVCLASS_CHR,
	.open = __kbdopen,
	.close = __close,
	.read = NOTSUP,
	.write = NOTSUP,
	.getc = __kbdgetc,
	.putc = NOTSUP
};

static int __driver_init(void)
{
	register_driver(MSIM_LP_MAJOR, &lpdrv);
	register_driver(MSIM_KBD_MAJOR, &kbddrv);
	spinlock_init(&cbuf.lock);
	add_interrupt_handler(__kbdintr, 3);
	/* TODO: add interrupt handler */
	return 0;
}
INITCALL_DRIVER(__driver_init);

