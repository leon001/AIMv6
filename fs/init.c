
#include <panic.h>
#include <fs/mount.h>
#include <fs/vnode.h>
#include <fs/vfs.h>

#include <fs/VOP.h>
#include <fs/namei.h>
#include <fs/ufs/inode.h>
#include <fs/ufs/ufsmount.h>
#include <fs/ufs/ext2fs/ext2fs.h>
#include <fs/bio.h>
#include <percpu.h>

void
fsinit(void)
{
	struct mount *rootmp;
	struct vnode *devvnode;
	struct vnode *tgz_vnode;
	struct vnode *lf_vnode;
	struct inode *tgz_inode;
	struct m_ext2fs *fs;
	struct buf *bp;
	soff_t blkno;

	kprintf("KERN: Starting FS...\n");
	mountroot();
	/* get root vnode */
	rootmp = list_first_entry(&mountlist, struct mount, node);
	kpdebug("rootmp at %p\n", rootmp);
	if (VFS_ROOT(rootmp, &rootvnode) != 0)
		panic("root vnode not found\n");
	vunlock(rootvnode);

	/*
	 * Replace this with your own test code...
	 */
	assert(rootvp->refs == 2);	/* one for device and one for / */
	fs = VFSTOUFS(rootmp)->superblock;
	/* Test VGET */
	assert(VFS_VGET(rootmp, 13890, &devvnode) == 0);
	assert(rootvp == devvnode);
	assert(devvnode->refs == 3);	/* another one for 13890 ref to same device */
	assert(rootvnode->refs == 1);
	vput(devvnode);
	assert(devvnode->refs == 2);
	assert(!(devvnode->flags & VXLOCK));
	assert(VFS_VGET(rootmp, 12, &tgz_vnode) == 0);
	assert(devvnode->refs == 3);	/* another one for 12 ref ON dev */
	assert(rootvnode->refs == 1);
	tgz_inode = VTOI(tgz_vnode);
	kpdebug("===========VGET done============\n");
	/* Test BMAP */
	for (int i = 0; i < tgz_inode->ndatablock; ++i) {
		assert(VOP_BMAP(tgz_vnode, i, NULL, &blkno, NULL) == 0);
		kpdebug("VOP_BMAP result: %d - %d (%d)\n", i, blkno,
		    dbtofsb(fs, blkno));
	}
	assert(VOP_BMAP(tgz_vnode, tgz_inode->ndatablock, NULL, &blkno, NULL) != 0);
	kpdebug("===========BMAP done============\n");
	/* Test bread on ext2fs files */
	assert(bread(tgz_vnode, 0, fs->bsize, &bp) == 0);
	kpdebug("Data read in: %08x\n", *(uint32_t *)bp->data);
	brelse(bp);
	vput(tgz_vnode);
	assert(devvnode->refs == 2);
	kpdebug("==========bread done============\n");
	/* Test LOOKUP (TODO: replace this with namei()) */
	vlock(rootvnode);
	assert(VOP_LOOKUP(rootvnode, "lost+found", &lf_vnode) == 0);
	assert(VTOI(lf_vnode)->ino == 11);
	assert(lf_vnode->type == VDIR);
	vput(lf_vnode);
	vunlock(rootvnode);
	kpdebug("==========LOOKUP done===========\n");
	/* Test namei */
	struct nameidata nd;
	nd.path = "/aimv6-0.1/msim.conf.in";
	nd.intent = NAMEI_LOOKUP;
	nd.flags = 0;
	assert(namei(&nd, current_proc) == 0);
	assert(VTOI(nd.vp)->ino == 27806);
	vput(nd.vp);
	kpdebug("==========namei done============\n");
	assert(devvnode->refs == 2);
	assert(rootvnode->refs == 1);

	kprintf("==============fs test succeeded===============\n");
}

