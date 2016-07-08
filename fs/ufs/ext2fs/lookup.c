
#include <fs/vnode.h>
#include <fs/bio.h>
#include <fs/ufs/inode.h>
#include <fs/ufs/ext2fs/dinode.h>
#include <fs/ufs/ext2fs/ext2fs.h>
#include <fs/ufs/ext2fs/dir.h>
#include <panic.h>
#include <libc/string.h>

int
ext2fs_lookup(struct vnode *dvp, char *name, struct vnode **vpp)
{
	unsigned int i;
	struct buf *bp;
	struct inode *ip = VTOI(dvp);
	struct m_ext2fs *fs = ip->superblock;
	struct ext2fs_dirhdr dh;
	void *cur;
	int err;
	char *entry_name;

	assert(dvp->type == VDIR);
	kpdebug("ext2fs lookup %p(%llu) %s\n", dvp, ip->ino, name);

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
			if (memcmp(entry_name, name, dh.namelen) == 0) {
				/* TODO NEXT */
				panic("ext2fs_lookup found %u\n", dh.ino);
			}
		}
		brelse(bp);
	}
	/* TODO NEXT */
	panic("ext2fs_lookup not found\n");
	vunlock(dvp);
}

