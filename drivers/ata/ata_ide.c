
#include <aim/device.h>
#include <aim/initcalls.h>
#include <sys/types.h>
#include <mach-conf.h>
#include <drivers/hd/hd.h>
#include <drivers/ata/ata.h>
#include <vmm.h>
#include <libc/string.h>
#include <fs/bio.h>
#include <errno.h>
#include <sleep.h>
#include <trap.h>

#define DEVICE_MODEL	"ide"

/*
 * Theoretically we can support multiple IDE drives (up to 4), but in our code
 * only one drive is supported.  With a preference over multiple drives for a
 * root drive, it will be easy to extend the code to multiple drives.
 */

static struct blk_driver drv;
static int port;	/* primary/secondary IDE, indicated by 0 or 2 */
static int slave;	/* master/slave IDE */

static int __init(struct devtree_entry *entry)
{
	struct bus_device *bus;
	struct hd_device *hd;
	bus_read_fp r8;
	bus_write_fp w8;
	uint64_t tmp;

	bus = (struct bus_device *)dev_from_name(entry->parent);
	r8 = bus->bus_driver.get_read_fp(bus, 8);
	w8 = bus->bus_driver.get_write_fp(bus, 8);
	for (port = 0; port < 4; port += 2) {
		for (slave = 0; slave <= 1; ++slave) {
			w8(bus, entry->regs[port], ATA_REG_DEVICE,
			    ATA_LBA | (slave ? ATA_DEV1 : 0));
			udelay(1000);
			r8(bus, entry->regs[port], ATA_REG_STATUS, &tmp);
			if ((tmp & ATA_DRDY) &&
			    !(tmp & (ATA_ERR | ATA_DF))) {
				kpdebug("Selecting %s %s IDE drive\n",
				    port ? "secondary" : "primary",
				    slave ? "slave" : "master");
				goto success;
			}
		}
	}

	panic("no IDE drive found\n");
	/* NOTREACHED */
	return 0;

success:
	hd = kmalloc(sizeof(*hd), GFP_ZERO);
	if (hd == NULL)
		return -ENOMEM;
	initdev(hd, DEVCLASS_BLK, entry->name,
	    make_hdbasedev(IDE_DISK_MAJOR, 0), &drv);
	hd->bus = bus;
	memcpy(hd->bases, entry->regs, sizeof(addr_t) * entry->nregs);
	hd->nregs = entry->nregs;
	list_init(&hd->bufqueue);
	dev_add(hd);
	return 0;
}

static int __intr(int irq)
{
	kpdebug("Handling ATA interrupt (IRQ %d)\n", irq);
	return 0;
}

static int __new(struct devtree_entry *entry)
{
	if (strcmp(entry->model, DEVICE_MODEL) != 0)
		return -ENOTSUP;
	/* assuming only one disk... */
	kpdebug("initializing IDE hard disk...\n");
	__init(entry);
	add_interrupt_handler(__intr, entry->irq);
	return 0;
}

static struct blk_driver drv = {
	.class = DEVCLASS_BLK,
	.new = __new,
};

static int __driver_init(void)
{
	register_driver(IDE_DISK_MAJOR, &drv);
	return 0;
}
INITCALL_DRIVER(__driver_init);

