
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
#include <aim/initcalls.h>

struct buf *buf_get(struct vnode *vp, off_t lblkno, size_t nblks);

/*
 * Find a struct buf in vnode @vp whose starting block is @blkno, spanning
 * @nblks blocks.  Mark the buf BUSY.
 *
 * bget() does not check for inconsistencies of number of blocks, overlaps
 * of cached buf's etc.
 */
struct buf *
bget(struct vnode *vp, off_t lblkno, size_t nblks)
{
	struct buf *bp;
	unsigned long flags;

	spin_lock_irq_save(&vp->buf_lock, flags);

	kprintf("DEBUG: bget %p %d %d\n", vp, lblkno, nblks);

restart:
	for_each_entry (bp, &vp->buf_head, node) {
		assert(bp->vnode == vp);
		if (bp->lblkno == lblkno) {
			assert(bp->nblks == nblks);
			if (!(bp->flags & B_BUSY)) {
				bp->flags |= B_BUSY;
				kprintf("DEBUG: bget found cached %p\n", bp);
				spin_unlock(&vp->buf_lock);
				return bp;
			}
			sleep_with_lock(bp, &vp->buf_lock);	/* brelse() */
			goto restart;
		}
	}

	bp = buf_get(vp, lblkno, nblks);
	spin_unlock_irq_restore(&vp->buf_lock, flags);

	return bp;
}

struct buf *
bgetempty(size_t nblks)
{
	kprintf("DEBUG: bgetempty() getting %d blocks\n", nblks);
	return buf_get(NULL, 0, nblks);
}

/*
 * Allocates a buf.
 * Assumes that the lock is held.
 */
struct buf *
buf_get(struct vnode *vp, off_t lblkno, size_t nblks)
{
	struct buf *bp;

	if (vp == NULL)
		goto create;	/* standalone buf */
	for_each_entry_reverse (bp, &vp->buf_head, node) {
		if (!(bp->flags & (B_BUSY | B_DIRTY))) {
			kfree(bp->data);
			kprintf("DEBUG: buf_get() recycle %p\n", bp);
			goto initbp;
		}
	}

create:
	bp = kmalloc(sizeof(*bp), 0);
	if (bp == NULL)
		return NULL;
	memset(bp, 0, sizeof(*bp));
	kprintf("DEBUG: buf_get() creating new %p\n", bp);
initbp:
	bp->flags |= B_BUSY | B_INVALID;
	bp->nblks = nblks;
	bp->blkno = BLKNO_INVALID;
	if (vp != NULL) {
		bp->lblkno = lblkno;
		bgetvp(vp, bp);
	}
	bp->data = kmalloc(BLOCK_SIZE * nblks, 0);

	return bp;
}

/*
 * Associate a buf with a vnode.
 */
void
bgetvp(struct vnode *vp, struct buf *bp)
{
	bp->vnode = vp;
	if (vp->type == VCHR || vp->type == VBLK)
		bp->devno = vp->specinfo->devno;
	list_add_tail(&(bp->node), &(vp->buf_head));
}

void
brelvp(struct buf *bp)
{
	kprintf("DEBUG: brelvp() releasing %p\n", bp);
	/* currently we do nothing */
}

static struct buf *
bio_doread(struct vnode *vp, off_t blkno, size_t nblks, uint32_t flags)
{
	struct buf *bp;

	bp = bget(vp, blkno, nblks);
	if ((bp->flags & B_INVALID) && !(bp->flags & B_DIRTY))
		VOP_STRATEGY(bp);

	return bp;
}

/*
 * Construct a buf and read the contents
 */
int
bread(struct vnode *vp, off_t blkno, size_t nblks, struct buf **bpp)
{
	struct buf *bp;

	bp = *bpp = bio_doread(vp, blkno, nblks, 0);
	return biowait(bp);
}

int
biowait(struct buf *bp)
{
	while (!(bp->flags & B_DONE))
		sleep(bp);	/* biodone() */
	if (bp->flags & B_EINTR) {
		bp->flags &= ~B_EINTR;
		return -EINTR;
	}
	if (bp->flags & B_ERROR)
		return bp->errno ? bp->errno : -EIO;
	return 0;
}

int
biodone(struct buf *bp)
{
	assert(!(bp->flags & B_DONE));
	bp->flags |= B_DONE;
	wakeup(bp);	/* biowait() */
	return 0;
}

/*
 * Release a buffer and make it available in buf cache, or free the buf if
 * it's standalone.
 */
void
brelse(struct buf *bp)
{
	unsigned long flags;
	bool standalone = (bp->vnode == NULL);
	assert(bp->flags & B_BUSY);

	/*
	 * brelse() does *NOT* check whether the I/O is truly finished.  It is
	 * left to the caller to ensure that the I/O is done, either succeeded
	 * or failed (XXX?).
	 * bread() and bwrite() automatically does that for you (by calling
	 * biowait()).  But some drivers may directly call the xxstrategy()
	 * calls, and in that case, biowait() should be called manually.
	 */

	kprintf("DEBUG: brelse() releasing %snode %p\n", standalone ? "standalone " : "", bp);

	if (!standalone)
		spin_lock_irq_save(&bp->vnode->buf_lock, flags);

	bp->flags &= ~B_BUSY;
	if (standalone) {
		kfree(bp->data);
		kfree(bp);
	} else {
		brelvp(bp);
	}
	wakeup(bp);	/* bget() */

	if (!standalone)
		spin_unlock_irq_restore(&bp->vnode->buf_lock, flags);
}

