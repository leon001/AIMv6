
#ifndef _FS_VOP_H
#define _FS_VOP_H

int VOP_OPEN(struct vnode *, int, struct ucred *, struct proc *);
int VOP_INACTIVE(struct vnode *, struct proc *);

#endif
