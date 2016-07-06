
#ifndef _FS_VOP_H
#define _FS_VOP_H

struct vnode;
struct ucred;
struct proc;
struct buf;

int VOP_OPEN(struct vnode *, int, struct ucred *, struct proc *);
int VOP_CLOSE(struct vnode *, int, struct ucred *, struct proc *);
int VOP_FSYNC(struct vnode *, struct ucred *, struct proc *);
int VOP_INACTIVE(struct vnode *, struct proc *);
int VOP_RECLAIM(struct vnode *);
int VOP_STRATEGY(struct buf *);

#endif
