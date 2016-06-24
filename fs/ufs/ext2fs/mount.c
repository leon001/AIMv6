
#include <fs/vnode.h>
#include <panic.h>

extern dev_t rootdev;	/* initialized in mach_init() or arch_init() */
extern struct vnode *rootvp;

int
ext2fs_mountroot(void)
{
	if (bdevvp(rootdev, &rootvp))
		panic("ext2fs_mountroot: can't setup bdevvp's\n");

	return 0;
}

