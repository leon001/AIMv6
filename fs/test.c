
#include <panic.h>
#include <fs/mount.h>
#include <fs/vnode.h>
#include <fs/vfs.h>
#include <fs/VOP.h>
#include <fs/namei.h>
#include <fs/uio.h>
#include <fs/ufs/inode.h>
#include <fs/ufs/ufsmount.h>
#include <fs/ufs/ext2fs/ext2fs.h>
#include <fs/bio.h>
#include <percpu.h>
#include <errno.h>
#include <ucred.h>

/*
 * Test routine
 */
void
fs_test(void)
{
	struct mount *rootmp = rootvnode->mount;
	struct vnode *devvnode;
	struct vnode *tgz_vnode;
	struct vnode *lf_vnode;
	struct inode *tgz_inode;
	struct m_ext2fs *fs;
	struct buf *bp;
	soff_t blkno;
	struct nameidata nd;
	struct uio uio;
	struct iovec iov[1];
	char *buf;
	int err;
	int i;

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
	nd.path = "/aimv6-0.1/../aimv6-0.1/msim.conf.in";
	nd.intent = NAMEI_LOOKUP;
	nd.flags = 0;
	assert(namei(&nd, current_proc) == 0);
	assert(VTOI(nd.vp)->ino == 27806);
	vput(nd.vp);
	assert(devvnode->refs == 2);
	assert(rootvnode->refs == 1);
	nd.path = "/aimv6-0.1/msim.conf.in";
	nd.intent = NAMEI_LOOKUP;
	nd.flags = 0;
	assert(namei(&nd, current_proc) == 0);
	assert(VTOI(nd.vp)->ino == 27806);
	vput(nd.vp);
	assert(devvnode->refs == 2);
	assert(rootvnode->refs == 1);
	nd.path = "/aimv6-0.1/../aimv6-0.1/msim.conf.in/";
	nd.intent = NAMEI_LOOKUP;
	nd.flags = 0;
	assert(namei(&nd, current_proc) == -ENOTDIR);
	assert(devvnode->refs == 2);
	assert(rootvnode->refs == 1);
	nd.path = "/aimv6-0.1/../aimv6-0.1/msim.conf";
	nd.intent = NAMEI_LOOKUP;
	nd.flags = 0;
	assert(namei(&nd, current_proc) == -ENOENT);
	assert(devvnode->refs == 2);
	assert(rootvnode->refs == 1);
	nd.path = "/aimv6-0.1";
	nd.intent = NAMEI_LOOKUP;
	nd.flags = 0;
	assert(namei(&nd, current_proc) == 0);
	assert(VTOI(nd.vp)->ino == 27777);
	vput(nd.vp);
	assert(devvnode->refs == 2);
	assert(rootvnode->refs == 1);
	nd.path = "/aimv6-0.1/";
	nd.intent = NAMEI_LOOKUP;
	nd.flags = 0;
	assert(namei(&nd, current_proc) == 0);
	assert(VTOI(nd.vp)->ino == 27777);
	vput(nd.vp);
	assert(devvnode->refs == 2);
	assert(rootvnode->refs == 1);
	kpdebug("==========namei done============\n");

	/* Test READ */
	nd.path = "/aimv6-0.1/configure";
	nd.intent = NAMEI_LOOKUP;
	nd.flags = 0;
	assert(namei(&nd, current_proc) == 0);
	assert(VTOI(nd.vp)->ino == 27988);
	buf = kmalloc(10010, 0);
	memset(buf, 0, 10010);
	iov[0].iov_base = buf;
	iov[0].iov_len = 10000;
	uio.iov = iov;
	uio.iovcnt = 1;
	uio.offset = 0;
	uio.resid = 10000;
	uio.rw = UIO_READ;
	uio.seg = UIO_KERNEL;
	uio.proc = NULL;
	assert(VOP_READ(nd.vp, &uio, 0, NOCRED) == 0);
	/* cannot kprintf() the whole buffer because kprintf() impose
	 * a string size limit */
	for (i = 0; i < 10000 && buf[i] != '\0'; ++i) {
		kprintf("%c", buf[i]);
	}
	kprintf("\n");
	assert(i == 10000);
	memset(buf, 0, 10010);
	iov[0].iov_base = buf;
	iov[0].iov_len = 10000;
	uio.iov = iov;
	uio.iovcnt = 1;
	uio.offset = 2000000;	/* Far far away from the end of file */
	uio.resid = 10000;
	uio.rw = UIO_READ;
	uio.seg = UIO_KERNEL;
	uio.proc = NULL;
	err = VOP_READ(nd.vp, &uio, 0, NOCRED);
	vput(nd.vp);
	assert(err == -E2BIG);	/* offset exceeds file size */
	kpdebug("===========READ done============\n");
}

