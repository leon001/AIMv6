
#include <panic.h>
#include <fs/mount.h>

void
fsinit(void)
{
	int err;
	mountroot();
	kprintf("==============fs test succeeded===============\n");
}

