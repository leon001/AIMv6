
#include <sys/types.h>
#include <sys/param.h>
#include <aim/device.h>
#include <libc/string.h>
#include <errno.h>
#include <vmm.h>
#include <aim/initcalls.h>
#include "introuter-ls3a.h"

#define DEVICE_MODEL	"introuter-ls3a"

static struct driver drv;
static struct device *introuter;

static int (*handlers[IR_SYSINTS])(void);

/*
 * The root interrupt controller is more or less coupled to concrete platform.
 */

static void __init(struct device *dev)
{
	/*
	 * Things are a little tricky here.
	 * Here I'm ensuring that only the master CPU handles I/O interrupt
	 * (including SATA, UART, USB, etc.).  Yes, we can make some of the
	 * slave CPUs to handle some of the I/O interrupts, but this will
	 * require sending IPIs to tell which CPU to handle which interrupts.
	 */
	struct bus_device *bus = dev->bus;
	bus_write_fp bus_write8 = bus->bus_driver.get_write_fp(bus, 8);
	bus_write_fp bus_write32 = bus->bus_driver.get_write_fp(bus, 32);
	bus_read_fp bus_read32 = bus->bus_driver.get_read_fp(bus, 32);
	uint64_t inten;
	bus_read32(bus, dev->base + IR_INTEN, &inten);
	/* Route LPC interrupts to CPU #0 IP2 */
	bus_write8(bus, dev->base + IR_LPC, IR_CPU(0) | IR_IP(2));
	inten |= 1 << IR_LPC;
	/* Route all HT1 interrupts to CPU #0 IP3 */
	for (int i = 0; i < 7; ++i) {
		bus_write8(bus, dev->base + IR_HT1_INTx(i), IR_CPU(0) | IR_IP(3));
		inten |= 1 << IR_HT1_INTx(i);
	}
	/* Enable all HT1 interrupts and LPC interrupt */
	bus_write32(bus, dev->base + IR_INTENSET, inten);
}

static int __new(struct devtree_entry *entry)
{
	struct device *dev;

	if (strcmp(entry->model, DEVICE_MODEL) != 0)
		return -ENOTSUP;
	kpdebug("initializing Loongson 3A interrupt router\n");
	dev = kmalloc(sizeof(*dev), GFP_ZERO);
	if (dev == NULL)
		return -ENOMEM;
	/* This device is not available to file system framework. */
	initdev(dev, DEVCLASS_NON, entry->name, NODEV, &drv);
	dev->bus = (struct bus_device *)dev_from_name(entry->parent);
	dev->base = entry->regs[0];
	dev->nregs = entry->nregs;
	dev_add(dev);
	__init(dev);
	introuter = dev;
	return 0;
}

static int __intr(void)
{
	struct bus_device *bus = introuter->bus;
	bus_read_fp bus_read32 = bus->bus_driver.get_read_fp(bus, 32);
	uint64_t isr;

	bus_read32(bus, introuter->base + IR_INTISR, &isr);

	/* Currently we only handle LPC and HT1 interrupts */
	for (int i = 0; i < IR_SYSINTS; ++i) {
		if (handlers[i] != NULL &&
		    (isr & (1 << i)))
			handlers[i]();
	}
	return 0;
}

static int __attach_intr(struct device *parent, struct device *child,
    int ncells, int *intr)
{
	assert(ncells == DEVTREE_INTR_AUTO);

	kpdebug("Attaching interrupts from %s to %s", child->name, parent->name);
	if (intr[0] == IR_LPC) {
		handlers[IR_LPC] = child->driver.intr;
	} else if (intr[0] == IR_HT1_INTx(0)) {
		for (int i = 0; i < 8; ++i)
			/* Assume that all HT1 interrupts share the same
			 * handler. */
			handlers[IR_HT1_INTx(i)] = child->driver.intr;
	} else {
		panic("introuter: unsupported ls3a interrupt %d\n", intr[0]);
	}
	return 0;
}

static struct driver drv = {
	.class = DEVCLASS_NON,
	.new = __new,
	.intr = __intr,
	.attach_intr = __attach_intr,
};

static int __driver_init(void)
{
	register_driver(NOMAJOR, &drv);
	return 0;
}
INITCALL_DRIVER(__driver_init);
