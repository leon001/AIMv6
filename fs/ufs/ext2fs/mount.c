
#include <fs/vnode.h>
#include <fs/specdev.h>
#include <fs/VOP.h>
#include <fs/mount.h>
#include <fs/vfs.h>
#include <aim/initcalls.h>
#include <panic.h>
#include <ucred.h>
#include <fcntl.h>
#include <proc.h>
#include <percpu.h>

extern dev_t rootdev;	/* initialized in mach_init() or arch_init() */
extern struct vnode *rootvp;

static struct vfsops ext2fs_vfsops = {
	0
};

int ext2fs_mountfs(struct vnode *, struct mount *, struct proc *);

int
ext2fs_mountroot(void)
{
	int err;
	struct mount *mp;

	if (bdevvp(rootdev, &rootvp))
		panic("ext2fs_mountroot: can't setup bdevvp's\n");

	if ((err = vfs_rootmountalloc("ext2fs", &mp)) != 0)
		goto rollback_rootvp;

	if ((err = ext2fs_mountfs(rootvp, mp, current_proc)) != 0)
		goto rollback_mountalloc;

	return 0;

rollback_mountalloc:
	vfs_mountfree(&mp);
rollback_rootvp:
	vrele(rootvp);
	return err;
}

int
ext2fs_mountfs(struct vnode *devvp, struct mount *mp, struct proc *p)
{
	int err;

	/*
	 * TODO:
	 * Disallow multiple mounts on the same device
	 * Disallow mounting a device in use
	 */

	err = VOP_OPEN(devvp, FREAD | FWRITE, NOCRED, p);
	return err;
}

int
ext2fs_register(void)
{
	registerfs("ext2fs", &ext2fs_vfsops);
	return 0;
}
INITCALL_FS(ext2fs_register);

