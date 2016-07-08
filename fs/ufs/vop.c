
#include <buf.h>
#include <fs/bio.h>
#include <fs/vnode.h>
#include <fs/VOP.h>
#include <fs/specdev.h>
#include <fs/ufs/inode.h>
#include <fs/ufs/ufsmount.h>
#include <panic.h>

/*
 * Execute an I/O request on a UFS vnode.
 *
 * We do it by translating the block I/O request on UFS to another block I/O
 * request on the device the UFS is mounted on.  That is, we only need to
 * translate logical block numbers to physical sector numbers on the
 * partition.
 */
int
ufs_strategy(struct buf *bp)
{
	struct vnode *vp = bp->vnode;
	struct inode *ip = VTOI(vp);
	unsigned long flags;
	int err;

	assert(vp->type != VBLK && vp->type != VCHR);

	/* NOTE: We only support reading one logical block at a time here, and
	 * we are NOT going to check it here */
	if (bp->blkno == BLKNO_INVALID) {
		err = VOP_BMAP(vp, bp->lblkno, NULL, &(bp->blkno), NULL);
		if (err) {
			bp->errno = err;
			bp->flags |= B_ERROR;
			local_irq_save(flags);
			biodone(bp);
			local_irq_restore(flags);
			return err;
		}
	}
	assert(bp->blkno != BLKNO_INVALID);
	vp = ip->ufsmount->devvp;
	bp->devno = vp->specinfo->devno;
	/* Here, we are actually calling spec_strategy() */
	return (vp->ops->strategy)(bp);
}

