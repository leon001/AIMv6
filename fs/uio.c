
#include <sys/types.h>
#include <fs/uio.h>
#include <vmm.h>
#include <proc.h>
#include <panic.h>
#include <util.h>
#include <libc/string.h>

static int
__uiomove(void *kbuf, void *kubuf, size_t len, enum uio_rw rw, enum uio_seg seg,
    struct proc *p)
{
	if (seg == UIO_USER) {
		assert(p != NULL);
		if (rw == UIO_READ)
			return copy_to_uvm(p->mm, kubuf, kbuf, len);
		else
			return copy_from_uvm(p->mm, kubuf, kbuf, len);
	} else {
		if (rw == UIO_READ)
			memcpy(kubuf, kbuf, len);
		else
			memcpy(kbuf, kubuf, len);
		return 0;
	}
}

/*
 * Moving @len bytes of data from/to one single buffer at @kbuf to/from a vector
 * of buffers at @uio->iov, either at kernel space or user space.
 *
 * When moving data from/to user space, uiomove() automatically deals with page
 * faults (including swapping, if we support it), by calling copy_to_uvm() and
 * copy_from_uvm().
 */
int
uiomove(void *kbuf, size_t len, struct uio *uio)
{
	size_t mvlen;
	int err;

	while (len > 0) {
		assert(uio->iovcnt > 0);
		mvlen = min2(len, uio->iov->iov_len);
		assert(uio->resid >= mvlen);
		kpdebug("uiomove: moving %ld bytes from %p to %p\n",
		    mvlen, kbuf, uio->iov->iov_base);
		err = __uiomove(kbuf, uio->iov->iov_base, mvlen, uio->rw,
		    uio->seg, uio->proc);
		if (err)
			return err;
		if (mvlen == uio->iov->iov_len) {
			/* we consumed an iovec, move to next one */
			uio->iov++;
			uio->iovcnt--;
		} else {
			uio->iov->iov_base += mvlen;
			uio->iov->iov_len -= mvlen;
		}
		uio->resid -= mvlen;
		len -= mvlen;
	}
	return 0;
}

