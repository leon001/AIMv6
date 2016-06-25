
#ifndef _FS_VFS_H
#define _FS_VFS_H

struct vfsconf {
	const char *name;
	struct vfsops *ops;
};

struct vfsops {
	int dummy;
};

#define MAX_VFSCONFS	5

void registerfs(const char *name, struct vfsops *ops);
struct vfsconf *findvfsconf(const char *name);

#endif
