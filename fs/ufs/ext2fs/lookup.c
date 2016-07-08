
#include <fs/vnode.h>
#include <fs/bio.h>
#include <fs/vfs.h>
#include <fs/ufs/inode.h>
#include <fs/ufs/ext2fs/dinode.h>
#include <fs/ufs/ext2fs/ext2fs.h>
#include <fs/ufs/ext2fs/dir.h>
#include <panic.h>
#include <libc/string.h>
#include <errno.h>

int
ext2fs_lookup(struct vnode *dvp, char *name, struct vnode **vpp)
{
	unsigned int i;
	struct vnode *vp;
	struct buf *bp;
	struct inode *ip = VTOI(dvp);
	struct m_ext2fs *fs = ip->superblock;
	struct ext2fs_dirhdr dh;
	void *cur;
	int err;
	char *entry_name;

	assert(dvp->type == VDIR);
	kpdebug("%s\n", name);
	kpdebug("ext2fs lookup %p(%d) %s\n", dvp, ip->ino, name);

	/*
	 * TODO:
	 * Now I only implement the LOOKUP operation.
	 * Will implement CREATE, RENAME and DELETE operations later.
	 */
	vlock(dvp);
	/* Do a linear search in the directory content: we read the data
	 * blocks one by one, look up the name, and return the vnode. */
	for (i = 0; i < ip->ndatablock; ++i) {
		err = bread(dvp, i, fs->bsize, &bp);
		if (err) {
			vunlock(dvp);
			return err;
		}
		/* Iterate over the list of directory entries */
		for (e2fs_load_dirhdr((cur = bp->data), &dh);
		     (cur < bp->data + fs->bsize) && (dh.reclen != 0);
		     e2fs_load_dirhdr((cur += EXT2FS_DIRSIZ(dh.namelen)), &dh)) {
			entry_name = cur + sizeof(dh);
			/* Found? */
			if (memcmp(entry_name, name, dh.namelen) == 0) {
				err = VFS_VGET(dvp->mount, dh.ino, &vp);
				*vpp = (err == 0) ? vp : NULL;
				brelse(bp);
				vunlock(dvp);
				return err;
			}
		}
		brelse(bp);
	}
	*vpp = NULL;
	vunlock(dvp);
	return -ENOENT;
}

