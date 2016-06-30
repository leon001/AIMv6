
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <panic.h>

extern struct vnode *rootvp;

void
fsinit(void)
{
	int err;

	/* FIXME: This will be changed to a file system detector routine */
	assert((err = ext2fs_mountroot()) == 0);

	kprintf("==============fs test succeeded===============\n");
}

