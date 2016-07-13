
#include <aim/device.h>
#include <aim/sync.h>
#include <sys/param.h>
#include <sys/types.h>
#include <libc/string.h>

struct driver *devsw[MAJOR_MAX];

void register_driver(unsigned int major, struct driver *drv)
{
	devsw[major] = drv;
}

void initdev(struct device *dev, const char *name, dev_t devno)
{
	dev->devno = devno;
	strlcpy(dev->name, name, DEV_NAME_MAX);
	spinlock_init(&dev->lock);
}
