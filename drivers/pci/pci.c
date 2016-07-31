
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

struct pci_driver_table_entry {
	uint16_t	vendor;
	uint16_t	device;
	char		model[DEV_NAME_MAX];
};

/* The table is a map between (vendor, device) and (name, model) */
static struct pci_driver_table_entry modeltable[] = {
	{0x1002, 0x4390, "ide"},
	{0x1002, 0x4391, "ahci"},	/* NYI */
};

static int __new(struct devtree_entry *entry)
{
	struct bus_device *dev;
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
	dev_add(dev);
	return 0;
}

static int __probe(struct bus_device *inst)
{
	/*
	 * In practice, we should enumerate over all combinations of
	 * bus, device and function, check the Vendor-Device pair
	 * inside, and find the appropriate driver.
	 *
	 * Here, we only consider one combination: Bus #0, Device 17,
	 * Function 0, which is SATA controller on AMD SB7xx southbridge.
	 */
	int bus, device, func;
	uint32_t tag;
	uint16_t vendor_id, device_id;
	struct devtree_entry entry;

	/* Should be for-loop(s) in practice */
	bus = 0;
	device = 17;
	func = 0;
	kprintf("Detecting PCI Bus %d Dev %d Func %d\n", bus, device, func);

	tag = make_pci_tag(bus, device, func);

	vendor_id = __read16(inst, tag, PCI_VENDOR_ID);
	device_id = __read16(inst, tag, PCI_DEVICE_ID);

	for (int i = 0; i < ARRAY_SIZE(modeltable); ++i) {
		if (vendor_id == modeltable[i].vendor &&
		    device_id == modeltable[i].device) {
			snprintf(entry.name, DEV_NAME_MAX, "pci:%d,%d,%d",
			    bus, device, func);
			strlcpy(entry.model, modeltable[i].model, DEV_NAME_MAX);
			strlcpy(entry.parent, inst->name, DEV_NAME_MAX);
			kpdebug("found device %s\n", entry.name);
			/* Currently we consider that all PCI devices have 6
			 * register spaces */
			entry.nregs = 6;
			for (int j = 0; j < 6; ++j) {
				entry.regs[j] = __read32(inst, tag,
				    PCI_BASE_ADDRESS_0 + 4 * j);
				kpdebug("BAR%d: %08x\n", j, entry.regs[j]);
			}
			entry.irq = __read8(inst, tag, PCI_INTERRUPT_LINE);
			kpdebug("IRQ: %d\n", entry.irq);
			discover_device(&entry);
		}
	}

	return 0;
}

static struct bus_driver drv = {
	.class = DEVCLASS_BUS,
	.new = __new,
	.probe = __probe
};

static int __driver_init(void)
{
	register_driver(NOMAJOR, &drv);
	return 0;
}
INITCALL_DRIVER(__driver_init);

