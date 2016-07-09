
#include <sys/types.h>
#include <fs/ufs/inode.h>
#include <fs/ufs/ufsmount.h>
#include <fs/ufs/ext2fs/dinode.h>
#include <fs/ufs/ext2fs/ext2fs.h>
#include <fs/vnode.h>
#include <fs/bio.h>
#include <buf.h>
#include <panic.h>
#include <errno.h>

static int
__ext2fs_bmap(struct inode *ip, off_t lblkno, int level, soff_t *fsblkno)
{
	struct buf *bp = NULL;
	struct vnode *devvp = ip->ufsmount->devvp;
	struct ext2fs_dinode *dip = ip->dinode;
	struct m_ext2fs *fs = ip->superblock;
	int err, i;
	uint32_t index;
	uint32_t *indices;
	uint32_t base = 1;
	off_t root = dip->blocks[NDADDR + level - 1];
	if (root == 0) {
		*fsblkno = 0;
		return 0;
	}

	for (i = 1; i < level; ++i)
		base *= NINDIR(fs);
	vlock(devvp);
	for (i = 0; i < level; ++i) {
		err = bread(devvp, fsbtodb(fs, root), fs->bsize, &bp);
		if (err) {
			vunlock(devvp);
			if (bp != NULL)
				brelse(bp);
			return err;
		}
		indices = bp->data;
		index = lblkno / base;
		lblkno %= base;
		root = indices[index];
		brelse(bp);
		bp = NULL;
		if (root == 0)
			break;
	}
	*fsblkno = root;
	vunlock(devvp);
	return 0;
}

int
ext2fs_bmap(struct vnode *vp, off_t lblkno, struct vnode **vpp,
    soff_t *blkno, int *runp)
{
	struct inode *ip = VTOI(vp);
	struct ext2fs_dinode *dip = ip->dinode;
	struct m_ext2fs *fs = ip->superblock;
	struct vnode *devvp = ip->ufsmount->devvp;
	soff_t fsblkno;
	int err = 0;

	if (vpp != NULL)
		*vpp = devvp;
	if (blkno == NULL)
		return 0;

	/*
	 * Translate the file logical block number to file system
	 * logical block number.
	 *
	 * We deal with the simplest cases first: when the logical
	 * block number is invalid, or we can directly address it.
	 */
	if (lblkno >= ip->ndatablock) {
		fsblkno = BLKNO_INVALID;
		err = -E2BIG;
		goto finish;
	} else if (lblkno < NDADDR) {
		fsblkno = dip->blocks[lblkno];
		goto finish;
	}
	/*
	 * Singly indirect block case.
	 */
	lblkno -= NDADDR;
	if (lblkno < NINDIR(fs)) {
		err = __ext2fs_bmap(ip, lblkno, 1, &fsblkno);
		goto finish;
	}
	/*
	 * Doubly indirect block case
	 */
	lblkno -= NINDIR(fs);
	if (lblkno < NINDIR(fs) * NINDIR(fs)) {
		err = __ext2fs_bmap(ip, lblkno, 2, &fsblkno);
		goto finish;
	}
	/*
	 * Triply indirect block case
	 */
	lblkno -= NINDIR(fs) * NINDIR(fs);
	if (lblkno < NINDIR(fs) * NINDIR(fs) * NINDIR(fs)) {
		err = __ext2fs_bmap(ip, lblkno, 3, &fsblkno);
		goto finish;
	}

	fsblkno = BLKNO_INVALID;
	err = -ENXIO;
finish:
	/* Translate a file system logical block number to disk sector
	 * number */
	*blkno = (fsblkno == BLKNO_INVALID) ? BLKNO_INVALID :
	    fsbtodb(fs, fsblkno);
	return err;
}

