
#include <panic.h>
#include <fs/mount.h>
#include <fs/vnode.h>
#include <fs/vfs.h>

#include <fs/ufs/ext2fs/ext2fs.h>

void
fsinit(void)
{
	struct mount *rootmp;
	struct vnode *devvnode;

	mountroot();
	/* get root vnode */
	rootmp = list_first_entry(&mountlist, struct mount, node);
	kpdebug("rootmp at %p\n", rootmp);
	if (VFS_ROOT(rootmp, &rootvnode) != 0)
		panic("root vnode not found\n");
	kprintf("==============fs test succeeded===============\n");
}

