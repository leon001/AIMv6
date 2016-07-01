File System Notes
======

**This notes is largely unfinished.**

The file system framework mainly comes from OpenBSD, which in turn comes from
BSD.  To me, OpenBSD code is simpler than Linux and FreeBSD, and works well
across all file systems just as any other Unix-like systems do.  In AIMv6 I
added some more constraints in order to ensure there is no race conditions
and therefore simplifying problems.

The following describes the file system model as well as details for managing
buffers, internal file representations, etc., some of which does not have
counterparts in OpenBSD in order to simplify implementation.

### Support

Currently, we only support ext2 rev. 0.  We may add FAT support in future.

Virtual File System (VFS) is ported from OpenBSD to AIMv6 in order to enable
the system to work across multiple file systems smoothly.  We largely
simplified the implementation though.

### Constraints

1. While OpenBSD has implemented reader-writer locks on vnodes, in AIMv6, we
  only have *exclusive* locks.
2. Any operation on a vnode requires the vnode to be locked, in order to
  prevent any possible race conditions and ensure serialization in a simple but
  brutal way.  This is accomplished via `vlock()` and `vunlock()` routines.
3. Any flag changes on a buf requires the interrupt to be disabled, in order
  to prevent race conditions where the flags are changed in an interrupt
  after reading the flag *AND* before doing anything with it.
  * Note that since the vnode of a buf is locked, other cores cannot do
    anything from outside.
