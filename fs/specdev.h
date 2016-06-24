
#ifndef _FS_SPECDEV_H
#define _FS_SPECDEV_H

#include <sys/types.h>
#include <list.h>

enum vtype;	/* fs/vnode.h */
struct vnode;	/* fs/vnode.h */

struct specinfo {
	dev_t		devno;
	struct vnode	*vnode;
	struct list_head	spec_node;
};

int getdevvp(dev_t, struct vnode **, enum vtype);

#endif
