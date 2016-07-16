
#include <fs/vnode.h>
#include <fs/bio.h>
#include <fs/mount.h>
#include <fs/ufs/inode.h>
#include <fs/ufs/ufsmount.h>
#include <fs/ufs/ext2fs/ext2fs.h>
#include <fs/ufs/ext2fs/dir.h>
#include <fs/ufs/ext2fs/dinode.h>
#include <bitmap.h>
#include <panic.h>
#include <errno.h>

/*
 * A simple brute-force search algorithm for available inode number.
 */
int
ext2fs_inode_alloc(struct vnode *dvp, int imode, struct vnode **vpp)
{
	struct vnode *devvp = VFSTOUFS(dvp->mount)->devvp;
	struct inode *pip;
	struct m_ext2fs *fs;
	struct buf *bp_ibitmap;
	ufsino_t ino = 0, ibase;
	int err, avail, i;

	pip = VTOI(dvp);
	fs = pip->superblock;
	if (fs->e2fs.ficount == 0)
		return -ENOSPC;

	for (i = 0; i < fs->ncg; ++i) {
		ibase = fs->e2fs.ipg * i;
		err = bread(devvp, fsbtodb(fs, fs->gd[i].i_bitmap), fs->bsize,
		    &bp_ibitmap);
		if (err)
			return err;
		/* Search for available ino in this cylinder group.
		 * Note that bitmap_xxx are 1-based. */
		avail = bitmap_find_first_zero_bit(bp_ibitmap->data,
		    fs->e2fs.ipg);
		while (avail < EXT2_FIRSTINO && avail != 0 && i == 0) {
			/* if ino is below FIRSTINO, discard and continue */
			avail = bitmap_find_next_zero_bit(bp_ibitmap->data,
			    fs->e2fs.ipg, avail);
		}
		if (avail == 0) {
			brelse(bp_ibitmap);
			continue;
		}
		ino = avail + ibase;
		break;
		/* NOTREACHED */
	}
	if (ino == 0) {
		/* No free inodes in bitmap but superblock says there ARE free
		 * inodes.  Here, we don't attempt to fix it. */
		kprintf("%s: inconsistent inode bitmap and superblock\n", __func__);
		return -ENOSPC;
	}

	/* We found a free inode # @ino. */
	atomic_set_bit(avail, bp_ibitmap->data);
	fs->e2fs.ficount--;
	fs->gd[i].nifree--;
	fs->fmod = 1;
	if ((imode & EXT2_IFMT) == EXT2_IFDIR)
		fs->gd[i].ndirs++;
	panic("done, found inode %d\n", ino);
}

