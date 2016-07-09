
#ifndef _FS_VOP_H
#define _FS_VOP_H

struct vnode;
struct ucred;
struct proc;
struct buf;
struct uio;

int VOP_NOP();
int VOP_OPEN(struct vnode *, int, struct ucred *, struct proc *);
int VOP_CLOSE(struct vnode *, int, struct ucred *, struct proc *);
int VOP_READ(struct vnode *, struct uio *, int, struct ucred *);
int VOP_FSYNC(struct vnode *, struct ucred *, struct proc *);
int VOP_INACTIVE(struct vnode *, struct proc *);
int VOP_RECLAIM(struct vnode *);
int VOP_STRATEGY(struct buf *);
int VOP_LOOKUP(struct vnode *, char *, struct vnode **);
int VOP_BMAP(struct vnode *, off_t, struct vnode **, soff_t *, int *);

#endif
