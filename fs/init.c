
#include <panic.h>
#include <fs/mount.h>
#include <fs/vnode.h>
#include <fs/vfs.h>

#include <fs/ufs/inode.h>
#include <fs/ufs/ufsmount.h>
#include <fs/ufs/ext2fs/ext2fs.h>
#include <percpu.h>
#include <fs/namei.h>

void
fsinit(void)
{
	struct mount *rootmp;

	kprintf("KERN: Starting FS...\n");
	mountroot();
	/* get root vnode */
	rootmp = list_first_entry(&mountlist, struct mount, node);
	kpdebug("rootmp at %p\n", rootmp);
	if (VFS_ROOT(rootmp, &rootvnode) != 0)
		panic("root vnode not found\n");
	vunlock(rootvnode);

	kprintf("==============fs test succeeded===============\n");
}

