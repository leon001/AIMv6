
#include <fs/vnode.h>
#include <fs/specdev.h>
#include <fs/VOP.h>
#include <fs/mount.h>
#include <fs/vfs.h>
#include <fs/bio.h>
#include <fs/ufs/ext2fs/ext2fs.h>
#include <aim/initcalls.h>
#include <panic.h>
#include <ucred.h>
#include <fcntl.h>
#include <proc.h>
#include <percpu.h>
#include <libc/string.h>

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

	if ((err = vfs_rootmountalloc("ext2fs", &mp)) != 0) {
		kprintf("DEBUG: rolling back rootvp\n");
		goto rollback_rootvp;
	}

	if ((err = ext2fs_mountfs(rootvp, mp, current_proc)) != 0) {
		kprintf("DEBUG: rolling back mountalloc\n");
		goto rollback_mountalloc;
	}

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
	struct buf *bp;
	struct ext2fs fs;

	/*
	 * TODO:
	 * Disallow multiple mounts on the same device
	 * Disallow mounting a device in use
	 */

	err = VOP_OPEN(devvp, FREAD | FWRITE, NOCRED, p);
	if (err != 0)
		return err;

	err = bread(devvp, SBOFF / BLOCK_SIZE, SBSIZE / BLOCK_SIZE, &bp);
	if (err != 0)
		return err;
	memcpy(&fs, bp->data, min2(sizeof(fs), SBSIZE));
	brelse(bp);
	assert(fs.e2fs_magic == E2FS_MAGIC);
	kprintf("DEBUG: ext2fs revision %d.%d\n", fs.e2fs_rev, fs.e2fs_minrev);
	return 0;
}

int
ext2fs_register(void)
{
	registerfs("ext2fs", &ext2fs_vfsops);
	return 0;
}
INITCALL_FS(ext2fs_register);

