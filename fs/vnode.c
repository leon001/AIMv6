
#include <sys/types.h>
#include <fs/vnode.h>
#include <fs/specdev.h>
#include <vmm.h>
#include <errno.h>
#include <libc/string.h>
#include <aim/sync.h>
#include <sched.h>
#include <panic.h>

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

