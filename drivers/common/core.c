
#include <aim/device.h>
#include <sys/param.h>
#include <sys/types.h>

struct driver *devsw[MAJOR_MAX];

void register_driver(unsigned int major, struct driver *drv)
{
	devsw[major] = drv;
}

