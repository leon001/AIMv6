
#include <sys/types.h>
#include <aim/device.h>
#include <aim/initcalls.h>
#include <libc/string.h>
#include <libc/stddef.h>
#include <errno.h>
#include <mach-conf.h>
#include <sched.h>
#include <trap.h>

#define DEVICE_MODEL	"ns16550"

static struct chr_driver drv;
static struct {
	char buf[BUFSIZ];
	int head;
	int tail;
	lock_t lock;
} cbuf = { {0}, 0, 0, EMPTY_LOCK(&cbuf.lock) };

static int __intr(int irq)
{
	struct chr_device *dev;
	unsigned char c;
	unsigned long flags;

	dev = (struct chr_device *)dev_from_id(makedev(UART_MAJOR, 0));
	assert(dev != NULL);
	c = __uart_ns16550_getchar(dev);
	spin_lock_irq_save(&cbuf.lock, flags);
	if (cbuf.head == (cbuf.tail + 1) % BUFSIZ) {
		spin_unlock_irq_restore(&cbuf.lock, flags);
		return 0;
	}
	cbuf.buf[cbuf.tail++] = c;
	cbuf.tail %= BUFSIZ;
	wakeup(&cbuf);
	spin_unlock_irq_restore(&cbuf.lock, flags);
	return 0;
}

static int __new(struct devtree_entry *entry)
{
	struct chr_device *dev;

	if (strcmp(entry->model, DEVICE_MODEL) != 0)
		return -ENOTSUP;
	kpdebug("initializing UART 16550\n");
	dev = kmalloc(sizeof(*dev), GFP_ZERO);
	if (dev == NULL)
		return -ENOMEM;
	initdev(dev, DEVCLASS_CHR, entry->name, makedev(UART_MAJOR, 0), &drv);
	dev->bus = (struct bus_device *)dev_from_name(entry->parent);
	dev->base = entry->regs[0];
	dev->nregs = entry->nregs;
	dev_add(dev);
	/* Flush FIFO XXX */
	kprintf("\n");
	__uart_ns16550_init(dev);
	__uart_ns16550_enable(dev);
	__uart_ns16550_enable_interrupt(dev);
	add_interrupt_handler(__intr, entry->irq);
	return 0;
}

static struct chr_driver drv = {
	.class = DEVCLASS_CHR,
	.new = __new,
};

static int __driver_init(void)
{
	register_driver(UART_MAJOR, &drv);
	return 0;
}
INITCALL_DRIVER(__driver_init);
