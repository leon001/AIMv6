
/*
 * This is the common code for all PCI buses.
 *
 * Configuration space reading and writing are provided in separate files
 * pci-mem.c and pci-io.c, depending on what bus the PCI bus is attached to,
 * or which platform the PCI bus is on.
 */

#include <aim/device.h>
#include <aim/initcalls.h>
#include <sys/types.h>
#include <sys/param.h>
#include <errno.h>
#include <drivers/pci/pci.h>
#include <libc/string.h>
#include <libc/stdio.h>
#include <panic.h>

#define DEVICE_MODEL	"pci"

static struct bus_driver drv;

static int __new(struct devtree_entry *entry)
{
	struct pci_bus_device *dev;
	if (strcmp(entry->model, DEVICE_MODEL) != 0)
		return -ENOTSUP;
	/* PCI bus requires both memory bus and I/O bus to work */
	if (dev_from_name("memory") == NULL ||
	    dev_from_name("portio") == NULL)
		return -ENODEV;
	kpdebug("initializing PCI bus\n");
	dev = kmalloc(sizeof(*dev), GFP_ZERO);
	if (dev == NULL)
		return -ENOMEM;
	initdev(dev, DEVCLASS_BUS, entry->name, NODEV, &drv);
	dev->bus = (struct bus_device *)dev_from_name(entry->parent);
	dev->base = entry->regs[0];
	dev->nregs = entry->nregs;

	/* All the __xxx are provided in sources which includes this file */
	dev->r8 = __read8;
	dev->r16 = __read16;
	dev->r32 = __read32;
	dev->w8 = __write8;
	dev->w16 = __write16;
	dev->w32 = __write32;

	dev_add(dev);
	return 0;
}

static struct bus_driver drv = {
	.class = DEVCLASS_BUS,
	.new = __new,
	.probe = pci_probe,
};

static int __driver_init(void)
{
	register_driver(NOMAJOR, &drv);
	return 0;
}
INITCALL_DRIVER(__driver_init);

