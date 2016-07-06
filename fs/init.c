
#include <panic.h>
#include <fs/mount.h>
#include <fs/vnode.h>
#include <fs/vfs.h>

void
fsinit(void)
{
	struct mount *rootmp;

	mountroot();
	/* get root vnode */
	rootmp = list_first_entry(&mountlist, struct mount, node);
	if (VFS_ROOT(rootmp, &rootvnode) != 0)
		panic("root vnode not found\n");
	kprintf("==============fs test succeeded===============\n");
}

