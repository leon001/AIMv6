
#ifndef _FS_VFS_H
#define _FS_VFS_H

#include <sys/types.h>

struct mount;	/* fs/mount.h */
struct vnode;	/* fs/vnode.h */

struct vfsconf {
	const char *name;
	struct vfsops *ops;
};

struct vfsops {
	/*
	 * root() - get root vnode.
	 * On UFS-like file systems, get the vnode for '/' directory.
	 */
	int (*root)(struct mount *mp, struct vnode **vpp);
	/*
	 * vget() - get vnode according to inode # (or file ID #)
	 * Not every file system has a file ID (e.g. FAT)
	 */
	int (*vget)(struct mount *mp, ino_t ino, struct vnode **vpp);
};

int VFS_ROOT(struct mount *mp, struct vnode **vpp);
int VFS_VGET(struct mount *mp, ino_t ino, struct vnode **vpp);

#define MAX_VFSCONFS	5

void registerfs(const char *name, struct vfsops *ops);
struct vfsconf *findvfsconf(const char *name);

#endif
