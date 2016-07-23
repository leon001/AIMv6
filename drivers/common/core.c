
#include <aim/device.h>
#include <aim/sync.h>
#include <sys/param.h>
#include <sys/types.h>
#include <libc/string.h>

/* drivers with associated major number */
struct driver *devsw[MAJOR_MAX];
#define DRIVERS_MAX	128
/* all drivers, including those without major numbers (e.g. bus drivers) */
struct driver *drivers[DRIVERS_MAX];
int ndrivers = 0;

void register_driver(unsigned int major, struct driver *drv)
{
	if (major != NOMAJOR)
		devsw[major] = drv;
	drivers[ndrivers++] = drv;
}

void initdev(struct device *dev, const char *name, dev_t devno)
{
	dev->devno = devno;
	strlcpy(dev->name, name, DEV_NAME_MAX);
	spinlock_init(&dev->lock);
}

bool __probe_devices(void)
{
	return false;
}

void probe_devices(void)
{
	while (__probe_devices())
		/* nothing */;
}

