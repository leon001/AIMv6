
#include <aim/initcalls.h>
#include <fs/vfs.h>
#include <fs/mount.h>
#include <fs/vnode.h>
#include <fs/ufs/ufs.h>
#include <fs/ufs/ext2fs/ext2fs.h>
#include <panic.h>

struct vfsops ext2fs_vfsops = {
	.root = ufs_root,
	.vget = ext2fs_vget
};

struct vops ext2fs_vops = {
	.inactive = ext2fs_inactive,
	.reclaim = ext2fs_reclaim
};

int
ext2fs_register(void)
{
	registerfs("ext2fs", &ext2fs_vfsops);
	register_mountroot(ext2fs_mountroot);
	return 0;
}
INITCALL_FS(ext2fs_register);

