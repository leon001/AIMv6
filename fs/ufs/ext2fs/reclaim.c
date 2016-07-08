
#include <fs/vnode.h>
#include <fs/ufs/inode.h>
#include <fs/ufs/ufs.h>
#include <fs/ufs/ufsmount.h>
#include <atomic.h>
#include <proc.h>
#include <panic.h>
#include <vmm.h>

int
ext2fs_inactive(struct vnode *vp, struct proc *p)
{
	/* TODO */
	kpdebug("ext2 deactivating vnode %p\n", vp);
	vunlock(vp);
	return 0;
}

int
ext2fs_reclaim(struct vnode *vp)
{
	struct inode *ip;

	kpdebug("ext2 reclaiming vnode %p\n", vp);

	assert(vp->refs == 0);
	ip = VTOI(vp);
	if (ip == NULL)
		return 0;
	ufs_ihashrem(ip);

	if (ip->ufsmount != NULL && ip->ufsmount->devvp != NULL) {
		assert(ip->ufsmount->devvp->refs > 1);
		atomic_dec(&ip->ufsmount->devvp->refs);
	}

	if (ip->dinode != NULL) {
		kfree(ip->dinode);
	}

	kfree(ip);
	vp->data = NULL;

	return 0;
}

