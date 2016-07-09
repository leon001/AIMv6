
#ifndef _FS_UIO_H
#define _FS_UIO_H

#include <sys/types.h>
#include <sys/uio.h>

enum uio_rw {
	UIO_READ,
	UIO_WRITE
};

enum uio_seg {
	UIO_USER,
	UIO_KERNEL
};

struct proc;	/* include/proc.h */

struct uio {
	/*
	 * @iov, @iovcnt, @resid indicates current UIO status, so their values
	 * are not preserved between uiomove() calls.
	 */
	struct iovec 	*iov;
	int		iovcnt;
	size_t		resid;	/* # of bytes remaining */
	/* The following members are preserved */
	off_t		offset;	/* offset to file */
	enum uio_rw	rw;
	enum uio_seg	seg;
	/* This member is probably read-only in context of UIO as well, unless
	 * page faulting (which UIO doesn't care) */
	struct proc	*proc;
};

int uiomove(void *kbuf, size_t len, struct uio *uio);

#endif

