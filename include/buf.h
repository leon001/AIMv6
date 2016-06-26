
#ifndef _BUF_H
#define _BUF_H

#include <sys/types.h>
#include <list.h>

#define BLOCK_SIZE	512

struct buf {
	dev_t		devno;
	off_t		blkno;		/* physical block number on dev */
	off_t		lblkno;		/* logical block number in vnode
					 * Note that blkno == lblkno on
					 * non-special vnode means that we
					 * need to perform a bmap() to find
					 * the true physical block number */
	size_t		nblks;
	uint32_t	flags;
#define B_DIRTY		0x1		/* need to write */
#define B_BUSY		0x2		/* in use */
#define B_INVALID	0x4		/* need to read if clean */
	/* Block I/O status flags set by interrupt callback */
#define B_DONE		0x8		/* done, either succeeded or failed */
#define B_EINTR		0x10		/* interrupted */
#define B_ERROR		0x20		/* failed */
	int		errno;		/* valid if B_ERROR, 0 if unsure */
	char		*data;
	struct vnode	*vnode;
	struct list_head node;		/* vnode buf list node */
	struct list_head ionode;	/* device io list node */
	size_t		nblksrem;	/* # of blocks not finished */
};

#endif
