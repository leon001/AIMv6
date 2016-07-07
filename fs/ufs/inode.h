
#ifndef _FS_UFS_INODE_H
#define _FS_UFS_INODE_H

#include <sys/types.h>
#include <aim/sync.h>
#include <fs/ufs/ufs.h>
#include <list.h>

struct inode {
	/* in-memory inode */
	struct list_head node;		/* UFS hash table entry node */
	struct vnode	*vnode;
	struct ufsmount	*ufsmount;
	ufsino_t	ino;
	dev_t		devno;
	int		effnlink;	/* inode nlink when I/O completes */
	void		*superblock;
	lock_t		lock;
	uint32_t	uid;
	uint32_t	gid;
	uint64_t	nblock;
	uint64_t	filesize;
	void		*dinode;	/* on-disk inode (dinode) */
};

#define VTOI(vp)	((struct inode *)((vp)->data))
#define ITOV(ip)	((ip)->vnode)

#endif
