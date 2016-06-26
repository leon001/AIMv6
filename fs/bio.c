
#include <sys/types.h>
#include <buf.h>
#include <fs/bio.h>
#include <fs/vnode.h>
#include <fs/specdev.h>
#include <fs/VOP.h>
#include <panic.h>
#include <aim/sync.h>
#include <sched.h>
#include <list.h>
#include <vmm.h>
#include <errno.h>
#include <libc/string.h>

/*
 * Find a struct buf in vnode @vp whose starting block is @blkno, spanning
 * @nblks blocks.  Mark the buf BUSY.
 */
struct buf *
bget(struct vnode *vp, off_t lblkno, size_t nblks)
{
	struct buf *bp;

	kprintf("DEBUG: bget() on vnode %p(%d) lblkno %d nblks %d\n",
	    vp, vp->type, lblkno, nblks);

	spin_lock(&(vp->buf_lock));

	/* 1st case: the block is already cached */
restart:
	for_each_entry (bp, &vp->buf_head, node) {
		if (bp->lblkno == lblkno) {
			/* sanity checks */
			assert(bp->nblks == nblks);
			assert(bp->vnode == vp);

			if (!(bp->flags & B_BUSY)) {
				bp->flags |= B_BUSY;
				spin_unlock(&vp->buf_lock);
				bp->nblksrem = bp->nblks;
				return bp;
			}
			sleep_with_lock(bp, &vp->buf_lock);
			goto restart;
		}
	}

	/* 2nd case: the block is not cached, but we have some clean and
	 * idle struct buf's for use.   In this case, we can safely
	 * free up the data buffer that struct buf is using. */
	for_each_entry (bp, &vp->buf_head, node) {
		if (!(bp->flags & B_BUSY) && !(bp->flags & B_DIRTY)) {
			assert(bp->vnode == vp);
			kfree(bp->data);
			goto init_bp;
		}
	}

	/* last case: no struct buf is available. */
	bp = kmalloc(sizeof(*bp), 0);
	if (bp == NULL)
		return NULL;
	memset(bp, 0, sizeof(*bp));
	bgetvp(vp, bp);
init_bp:
	bp->lblkno = bp->blkno = lblkno;
	bp->nblksrem = bp->nblks = nblks;
	bp->flags = B_BUSY | B_INVALID;
	bp->data = kmalloc(BLOCK_SIZE * nblks, 0);

	spin_unlock(&(vp->buf_lock));
	return bp;
}

/*
 * Associate a buf with a vnode.
 */
void
bgetvp(struct vnode *vp, struct buf *bp)
{
	kprintf("bgetvp on bp %p and vnode %p (%d)\n", bp, vp, vp->type);
	assert(bp->vnode == NULL);
	bp->vnode = vp;
	if (vp->type == VCHR || vp->type == VBLK)
		bp->devno = vdev(vp);
	list_add_tail(&(bp->node), &(vp->buf_head));
}

static struct buf *
bio_doread(struct vnode *vp, off_t blkno, size_t nblks, uint32_t flags)
{
	struct buf *bp;

	bp = bget(vp, blkno, nblks);
	assert(bp != NULL);
	assert(VOP_STRATEGY(bp) == 0);

	return bp;
}

int
bread(struct vnode *vp, off_t blkno, size_t nblks, struct buf **bpp)
{
	struct buf *bp;

	kprintf("DEBUG: reading %d blocks at %d\n", nblks, blkno);

	bp = *bpp = bio_doread(vp, blkno, nblks, 0);
	return biowait(bp);
}

int
biowait(struct buf *bp)
{
	while (!(bp->flags & B_DONE))
		sleep(bp);
	if (bp->flags & B_EINTR) {
		bp->flags &= ~B_EINTR;
		return -EINTR;
	}
	if (bp->flags & B_ERROR)
		return bp->errno ? bp->errno : -EIO;
	return 0;
}

