
#include <sys/types.h>
#include <fs/vnode.h>
#include <fs/specdev.h>
#include <fs/mount.h>
#include <fs/VOP.h>
#include <fs/bio.h>
#include <vmm.h>
#include <errno.h>
#include <libc/string.h>
#include <aim/sync.h>
#include <aim/initcalls.h>
#include <sched.h>
#include <panic.h>
#include <proc.h>
#include <percpu.h>
#include <atomic.h>
#include <ucred.h>

struct allocator_cache vnodepool = {
	.size = sizeof(struct vnode),
	.align = 1,
	.flags = 0,
	.create_obj = NULL,
	.destroy_obj = NULL
};

struct vnode *rootvp;
struct vnode *rootvnode;

int
vnodeinit(void)
{
	spinlock_init(&vnodepool.lock);
	assert(cache_create(&vnodepool) == 0);
	return 0;
}
INITCALL_FS(vnodeinit);

/* Allocate a vnode mounted on @mp with operations @ops. */
int
getnewvnode(struct mount *mp, struct vops *ops, struct vnode **vpp)
{
	struct vnode *vp;

	vp = cache_alloc(&vnodepool);
	if (vp == NULL) {
		*vpp = NULL;
		return -ENOMEM;
	}
	memset(vp, 0, sizeof(*vp));

	vp->ops = ops;
	vp->refs = 1;
	vp->type = VNON;
	vp->data = NULL;
	vp->mount = NULL;
	vp->noutputs = 0;
	list_init(&vp->buf_head);
	list_init(&vp->mount_node);

	insmntque(vp, mp);

	*vpp = vp;
	return 0;
}

/* Lock a vnode for exclusive access */
void
vlock(struct vnode *vp)
{
	spin_lock(&(vp->lock));
	while (vp->flags & VXLOCK)
		sleep_with_lock(vp, &(vp->lock));
	vp->flags |= VXLOCK;
	spin_unlock(&(vp->lock));
}

/* Unlock a vnode */
void
vunlock(struct vnode *vp)
{
	assert(vp->flags & VXLOCK);

	spin_lock(&(vp->lock));
	vp->flags &= ~VXLOCK;
	wakeup(vp);
	spin_unlock(&(vp->lock));
}

/*
 * Destroy a vnode.
 * Must be inactivated before calling.
 */
void
vgone(struct vnode *vp)
{
	unsigned long flags;

	kpdebug("destroying vnode %p\n", vp);
	/*
	 * vgone() is executed only if refs goes down to 0.  So if vgone()
	 * found that the vnode is locked, there must be another vgone()
	 * going on.
	 * FIXME: do we need to wait for vgone() to finish here?
	 */
	spin_lock_irq_save(&(vp->lock), flags);
	if (vp->flags & VXLOCK) {
		assert(vp->refs == 0);
		spin_unlock_irq_restore(&(vp->lock), flags);
		return;
	}
	vp->flags |= VXLOCK;
	spin_unlock_irq_restore(&(vp->lock), flags);

	/* FIXME: replace with suitable ucred and proc */
	vinvalbuf(vp, NOCRED, current_proc);

	assert(VOP_RECLAIM(vp) == 0);

	if (vp->mount != NULL)
		insmntque(vp, NULL);
	/* Clean up specinfo */
	if ((vp->type == VBLK || vp->type == VCHR) && vp->specinfo != NULL) {
		list_del(&vp->specinfo->spec_node);
		cache_free(&specinfopool, vp->specinfo);
	}
	/* Free vnode */
	cache_free(&vnodepool, vp);
}

/*
 * vref() - increment reference count
 */
void
vref(struct vnode *vp)
{
	atomic_inc(&(vp->refs));
}

void
vget(struct vnode *vp)
{
	vlock(vp);
	atomic_inc(&(vp->refs));
}

/*
 * vput() - unlock and release.  Of course, assumes the vnode to be locked.
 */
void
vput(struct vnode *vp)
{
	atomic_dec(&(vp->refs));
	if (vp->refs > 0) {
		vunlock(vp);
		return;
	}
	VOP_INACTIVE(vp, current_proc);	/* unlock */
	vgone(vp);
}

int
vrele(struct vnode *vp)
{
	kpdebug("vrele %p\n", vp);
	atomic_dec(&(vp->refs));
	if (vp->refs > 0)
		return 0;
	vlock(vp);
	VOP_INACTIVE(vp, current_proc);	/* unlock */
	vgone(vp);
	return 0;
}

/* Assume vnode is locked */
int
vwaitforio(struct vnode *vp)
{
	kpdebug("vwaitforio %p\n", vp);
	while (vp->noutputs) {
		sleep(&vp->noutputs);
	}
	kpdebug("vwaitforio %p done\n", vp);
	return 0;
}

/*
 * Invalidate all buf's in vnode @vp:
 * Wait for all I/O's to finish, sync all dirty buf's, clean the vnode buf list
 * and free all elements inside.
 * Assumes vnode is locked.
 */
int
vinvalbuf(struct vnode *vp, struct ucred *cred, struct proc *p)
{
	struct buf *bp, *bnext;
	unsigned long flags;

	kpdebug("vinvalbuf %p\n", vp);

	vwaitforio(vp);
	VOP_FSYNC(vp, cred, p);

	local_irq_save(flags);
loop:
	for_each_entry_safe (bp, bnext, &vp->buf_head, node) {
		if (bp->flags & B_BUSY) {
			sleep(bp);
			goto loop;
		}
		assert(!(bp->flags & B_DIRTY));
		list_del(&bp->node);
		bdestroy(bp);
	}
	local_irq_restore(flags);

	kpdebug("vinvalbuf %p done\n", bp);
	return 0;
}

