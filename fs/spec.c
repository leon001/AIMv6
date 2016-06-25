
#include <list.h>
#include <fs/vnode.h>
#include <fs/specdev.h>
#include <vmm.h>
#include <errno.h>

static struct list_head specinfo_list = EMPTY_LIST(specinfo_list);
static struct vops spec_vops;	/* forward declaration */

int
spec_inactive(struct vnode *vp, struct proc *p)
{
	/* For a special vnode we just unlock it */
	vunlock(vp);
	return 0;
}

int
spec_open(struct vnode *vp, int mode, struct ucred *cred, struct proc *p)
{
	switch (vp->type) {
	case VCHR:
		/* TODO, currently fallthru */
	case VBLK:
		/* TODO: find and invoke driver */
		return 0;
	default:
		return -ENODEV;
	}
	return -EINVAL;
}

/*
 * Get or create the vnode corresponding to the device @devno.
 * Currently we require that each device is matched exactly by one vnode.
 */
int
getdevvp(dev_t devno, struct vnode **vpp, enum vtype type)
{
	struct specinfo *si;
	struct vnode *vp;
	int err;

	for_each_entry (si, &specinfo_list, spec_node) {
		if (si->devno == devno) {
			*vpp = si->vnode;
			return 0;
		}
	}

	err = getnewvnode(NULL, &spec_vops, &vp);
	if (err != 0) {
		*vpp = NULL;
		return err;
	}

	vp->type = type;
	/* Allocate and fill in a specinfo structure */
	si = kmalloc(sizeof(*si), 0);
	si->devno = devno;
	si->vnode = vp;
	list_add_tail(&(si->spec_node), &specinfo_list);
	vp->specinfo = si;

	*vpp = vp;
	return 0;
}

/* Get or create a block device vnode */
int
bdevvp(dev_t devno, struct vnode **vpp)
{
	return getdevvp(devno, vpp, VBLK);
}

dev_t
vdev(struct vnode *vp)
{
	if (vp->type != VCHR && vp->type != VBLK)
		return NODEV;
	else
		return vp->specinfo->devno;
}

static struct vops spec_vops = {
	.open = spec_open,
	.inactive = spec_inactive,
};

