
#include <aim/initcalls.h>
#include <fs/vfs.h>
#include <fs/mount.h>

extern struct vfsops ext2fs_vfsops;
extern int ext2fs_mountroot(void);

int
ext2fs_register(void)
{
	registerfs("ext2fs", &ext2fs_vfsops);
	register_mountroot(ext2fs_mountroot);
	return 0;
}
INITCALL_FS(ext2fs_register);

