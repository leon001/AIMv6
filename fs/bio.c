
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

static struct list_head bufcache;
static lock_t bufcache_lock;

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

	spin_lock_irq_save(&bufcache_lock, flags);

restart:
	for_each_entry (bp, &bufcache, node) {
		assert(bp->vnode != NULL);
		if (bp->vnode == vp && bp->lblkno == lblkno) {
			assert(bp->nblks == nblks);
			if (!(bp->flags & B_BUSY)) {
				bp->flags |= B_BUSY;
				spin_unlock(&bufcache_lock);
				return bp;
			}
			sleep_with_lock(bp, &bufcache_lock);
			goto restart;
		}
	}

	bp = buf_get(vp, lblkno, nblks);
	spin_unlock_irq_restore(&bufcache_lock, flags);

	return bp;
}

struct buf *
bgetempty(size_t nblks)
{
	kprintf("bgetempty() getting %d blocks\n", nblks);
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

	for_each_entry_reverse (bp, &bufcache, node) {
		if (!(bp->flags & (B_BUSY | B_DIRTY))) {
			kfree(bp->data);
			goto initbp;
		}
	}

	bp = kmalloc(sizeof(*bp), 0);
	memset(bp, 0, sizeof(*bp));
initbp:
	bp->flags |= B_BUSY | B_INVALID;
	bp->nblks = nblks;
	if (vp != NULL) {
		list_add_tail(&(bp->node), &bufcache);
		bp->lblkno = bp->blkno = lblkno;
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
}

static struct buf *
bio_doread(struct vnode *vp, off_t blkno, size_t nblks, uint32_t flags)
{
	panic("bio_doread() NYI\n");
	return NULL;
}

int
bread(struct vnode *vp, off_t blkno, size_t nblks, struct buf **bpp)
{
	panic("bread() NYI\n");
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

int
binit(void)
{
	list_init(&bufcache);
	spinlock_init(&bufcache_lock);
	return 0;
}
INITCALL_SUBSYS(binit);

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

	kprintf("brelse() releasing %snode %p\n", standalone ? "standalone " : "", bp);

	spin_lock_irq_save(&bufcache_lock, flags);

	bp->flags &= ~B_BUSY;

	if (standalone) {
		kfree(bp->data);
		kfree(bp);
	}

	spin_unlock_irq_restore(&bufcache_lock, flags);
}

/*
 * Helper function called when destroying @vp.  Removes all buf's with vnode
 * @vp.
 */
void
bufcache_remove_vnode(struct vnode *vp)
{
	struct buf *bp, *bnext;
	for_each_entry_safe (bp, bnext, &bufcache, node) {
		if (bp->vnode == vp) {
			assert(bp->data != NULL);
			assert(!(bp->flags & B_BUSY));
			kfree(bp->data);
			list_del(&(bp->node));
			kfree(bp);
		}
	}
}

