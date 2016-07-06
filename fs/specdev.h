
#ifndef _FS_SPECDEV_H
#define _FS_SPECDEV_H

#include <sys/types.h>
#include <list.h>
#include <vmm.h>

enum vtype;	/* fs/vnode.h */
struct vnode;	/* fs/vnode.h */
struct vops;	/* fs/vnode.h */

struct specinfo {
	dev_t		devno;
	struct vnode	*vnode;
	struct mount	*mountpoint;

	struct list_head spec_node;
};

int bdevvp(dev_t, struct vnode **);
int getdevvp(dev_t, struct vnode **, enum vtype);
struct specinfo *findspec(dev_t);
dev_t vdev(struct vnode *);

extern struct allocator_cache specinfopool;
extern struct vops spec_vops;

#endif
