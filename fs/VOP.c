
#include <fs/vnode.h>
#include <proc.h>
#include <errno.h>

int VOP_INACTIVE(struct vnode *vp, struct proc *p)
{
	if (vp->ops->inactive == NULL)
		return -ENOTSUP;
	return (vp->ops->inactive)(vp, p);
}

int VOP_OPEN(struct vnode *vp, int mode, struct ucred *cred, struct proc *p)
{
	if (vp->ops->open == NULL)
		return -ENOTSUP;
	return (vp->ops->open)(vp, mode, cred, p);
}

