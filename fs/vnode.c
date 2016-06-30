
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
#include <sched.h>
#include <panic.h>
#include <proc.h>
#include <percpu.h>
#include <atomic.h>

/* FIXME */
struct vnode *rootvp;

/* Allocate a vnode mounted on @mp (unused now) with operations @ops. */
int
getnewvnode(struct mount *mp, struct vops *ops, struct vnode **vpp)
{
	struct vnode *vp;

	vp = kmalloc(sizeof(*vp), 0);
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
	assert(vp->refs > 0);
	assert(vp->flags & VXLOCK);

	spin_lock(&(vp->lock));
	vp->flags &= ~VXLOCK;
	wakeup(vp);
	spin_unlock(&(vp->lock));
}

int
vgone(struct vnode *vp)
{
	panic("vgone NYI\n");
	return 0;
}

int
vrele(struct vnode *vp)
{
	atomic_dec(&(vp->refs));
	if (vp->refs > 0)
		return 0;
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

