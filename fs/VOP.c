
#include <fs/vnode.h>
#include <proc.h>
#include <errno.h>

int VOP_OPEN(struct vnode *vp, int mode, struct ucred *cred, struct proc *p)
{
	if (vp->ops->open == NULL)
		return -ENOTSUP;
	return (vp->ops->open)(vp, mode, cred, p);
}

int VOP_CLOSE(struct vnode *vp, int mode, struct ucred *cred, struct proc *p)
{
	if (vp->ops->close == NULL)
		return -ENOTSUP;
	return (vp->ops->close)(vp, mode, cred, p);
}

int VOP_FSYNC(struct vnode *vp, struct ucred *cred, struct proc *p)
{
	/* Currently we are all doing sync'ed operations, so we do nothing */
	return 0;
}

int VOP_INACTIVE(struct vnode *vp, struct proc *p)
{
	if (vp->ops->inactive == NULL)
		return -ENOTSUP;
	return (vp->ops->inactive)(vp, p);
}

int VOP_RECLAIM(struct vnode *vp)
{
	if (vp->ops->reclaim == NULL)
		return -ENOTSUP;
	return (vp->ops->reclaim)(vp);
}

int VOP_STRATEGY(struct buf *bp)
{
	if (bp->vnode->ops->strategy == NULL)
		return -ENOTSUP;
	return (bp->vnode->ops->strategy)(bp);
}

