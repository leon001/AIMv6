
#ifndef _FS_VNODE_H
#define _FS_VNODE_H

#include <sys/types.h>

struct specinfo;	/* fs/specdev.h */
struct mount;		/* fs/mount.h */

enum vtype {
	VNON,	/* no-type */
	VREG,	/* regular file */
	VDIR,	/* directory */
	VBLK,	/* block device */
	VCHR,	/* character device */
	VLNK,	/* symlink */
	VBAD	/* ??? */
};

struct vnode {
	enum vtype	type;
	struct vops	*ops;
	atomic_t	refs;
	union {
		struct specinfo *specinfo;
		void *typedata;
	};
	/* FS-specific data.  For UFS-like filesystems this points to inode. */
	void		*data;
};

struct vops {
	int dummy;
};

int bdevvp(dev_t, struct vnode **);
int getnewvnode(struct mount *, struct vops *, struct vnode **);

#endif
