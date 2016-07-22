
#include <sys/types.h>
#include <fs/vnode.h>
#include <fs/uio.h>
#include <fs/bio.h>
#include <fs/ufs/inode.h>
#include <fs/ufs/ext2fs/ext2fs.h>
#include <buf.h>
#include <ucred.h>
#include <panic.h>

int
ext2fs_read(struct vnode *vp, struct uio *uio, int ioflags, struct ucred *cred)
{
	struct buf *bp = NULL;
	struct m_ext2fs *fs = VTOI(vp)->superblock;
	off_t fsb;
	size_t len = 0, b_off;
	int i;
	int err;

	/* sanity checks */
	for (i = 0; i < uio->iovcnt; ++i)
		len += uio->iov[i].iov_len;
	assert(len == uio->resid);
	assert(uio->rw == UIO_READ);

	fsb = lblkno(fs, uio->offset);
	b_off = lblkoff(fs, uio->offset);

	while (uio->resid > 0) {
		err = bread(vp, fsb, fs->bsize, &bp);
		if (err) {
			brelse(bp);
			return err;
		}
		len = min2(uio->resid, fs->bsize - b_off);
		err = uiomove(bp->data + b_off, len, uio);
		if (err) {
			brelse(bp);
			return err;
		}
		b_off = 0;
		++fsb;
		brelse(bp);
	}

	VTOI(vp)->flags |= IN_ACCESS;
	return 0;
}

#if 0
int
ext2fs_write(struct vnode *vp, struct uio *uio, int ioflags, struct ucred *cred)
{
	struct inode *ip = VTOI(vp);
	struct m_ext2fs *fs = ip->superblock;
	size_t len = 0;
	int i, err;

	/* sanity checks */
	for (i = 0; i < uio->iovcnt; ++i)
		len += uio->iov[i].iov_len;
	assert(len == uio->resid);
	assert(uio->rw == UIO_WRITE);

	/*
	 * If writing 0 bytes, do not change update time or file offset
	 * (standards compliance)
	 */
	if (uio->resid == 0)
		return 0;

	switch (vp->type) {
	case VREG:
		if (ioflags & IO_APPEND)
			uio->offset = ext2fs_getsize(ip);
		/* fallthru */
	case VLNK:
	case VDIR:
		break;
	default:
		panic("%s: bad type %d\n", __func__, vp->type);
	}

	if (uio->offset + uio->resid < uio->resid ||	/* overflowing */
	    uio->offset + uio->resid > fs->maxfilesize)
		return -EFBIG;

	resid = uio->resid;
	osize = ext2fs_getsize(ip);

	for (err = 0; uio->resid > 0; ) {
		lbn = lblkno(fs, uio->offset);
		blkoffset = blkoff(fs, uio->offset);
		xfersize = min2(uio->resid, fs->bsize - blkoffset);

		err = ext2fs_buf_alloc(ip, lbn, cred, &bp);
	}
}
#endif
