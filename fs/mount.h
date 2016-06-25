
#ifndef _FS_MOUNT_H
#define _FS_MOUNT_H

#include <list.h>
#include <sys/types.h>

struct mount {
	struct list_head vnode_head;
	struct vfsops	*ops;
	/* Mount lock */
	lock_t		lock;
	uint32_t	flags;
#define MOUNT_BUSY	0x1
	void		*data;
};

void insmntque(struct vnode *vp, struct mount *mp);
int vfs_rootmountalloc(const char *fsname, struct mount **mpp);
void vfs_mountfree(struct mount **mp);
int vfs_busy(struct mount *mp, bool write, bool sleep);
void vfs_unbusy(struct mount *mp);

#endif
