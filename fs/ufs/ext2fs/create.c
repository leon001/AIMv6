
#include <fs/vnode.h>
#include <fs/ufs/ext2fs/dinode.h>
#include <panic.h>

int
ext2fs_makeinode(int imode, struct vnode *dvp, char *name, struct vnode **vpp)
{
	int err;
	struct vnode *tvp;

	assert((imode & EXT2_IFMT) != 0);

	err = ext2fs_inode_alloc(dvp, imode, &tvp);
	if (err)
		return err;

	panic("makeinode done\n");
}

int
ext2fs_create(struct vnode *dvp, char *name, int mode, struct vnode **vpp)
{
	return ext2fs_makeinode(EXT2_MAKEIMODE(VREG, mode), dvp, name, vpp);
}

