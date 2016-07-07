
#include <panic.h>
#include <fs/mount.h>
#include <fs/vnode.h>
#include <fs/vfs.h>
#include <fs/VOP.h>

void
fsinit(void)
{
	struct mount *rootmp;
	struct vnode *devvnode;
	struct vnode *tgz_vnode;
	soff_t blkno;

	mountroot();
	/* get root vnode */
	rootmp = list_first_entry(&mountlist, struct mount, node);
	kpdebug("rootmp at %p\n", rootmp);
	if (VFS_ROOT(rootmp, &rootvnode) != 0)
		panic("root vnode not found\n");

	/*
	 * Replace this with your own test code...
	 */
	assert(VFS_VGET(rootmp, 13890, &devvnode) == 0);
	assert(devvnode->refs == 2);
	vput(devvnode);
	assert(devvnode->refs == 1);
	assert(!(devvnode->flags & VXLOCK));
	assert(VFS_VGET(rootmp, 12, &tgz_vnode) == 0);
	assert(VOP_BMAP(tgz_vnode, 12, NULL, &blkno, NULL) == 0);
	kpdebug("VOP_BMAP result: %d\n", blkno);

	kprintf("==============fs test succeeded===============\n");
}

