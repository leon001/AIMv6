
#include <panic.h>
#include <fs/mount.h>
#include <fs/vnode.h>
#include <fs/vfs.h>

#include <fs/VOP.h>
#include <fs/ufs/inode.h>
#include <fs/ufs/ufsmount.h>
#include <fs/ufs/ext2fs/ext2fs.h>

void
fsinit(void)
{
	struct mount *rootmp;
	struct vnode *devvnode;
	struct vnode *tgz_vnode;
	struct inode *tgz_inode;
	struct m_ext2fs *fs;
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
	fs = ((struct ufsmount *)rootmp->data)->superblock;
	assert(VFS_VGET(rootmp, 13890, &devvnode) == 0);
	assert(devvnode->refs == 2);
	vput(devvnode);
	assert(devvnode->refs == 1);
	assert(!(devvnode->flags & VXLOCK));
	assert(VFS_VGET(rootmp, 12, &tgz_vnode) == 0);
	tgz_inode = tgz_vnode->data;
	for (int i = 0; i < tgz_inode->ndatablock; ++i) {
		assert(VOP_BMAP(tgz_vnode, i, NULL, &blkno, NULL) == 0);
		kpdebug("VOP_BMAP result: %d - %d (%d)\n", i, blkno,
		    dbtofsb(fs, blkno));
	}
	assert(VOP_BMAP(tgz_vnode, tgz_inode->ndatablock, NULL, &blkno, NULL) != 0);

	kprintf("==============fs test succeeded===============\n");
}

