File System
======

**This note is largely unfinished.**

The file system framework mainly comes from OpenBSD, which in turn comes from
BSD.  To me, OpenBSD code is simpler than Linux and FreeBSD, and works well
across all file systems just as any other Unix-like systems do.  In AIMv6 I
added some more constraints in order to ensure there is no race conditions
and therefore simplifying problems.

The following describes the file system model as well as details for managing
buffers, internal file representations, etc., some of which does not have
counterparts in OpenBSD in order to simplify implementation.

## Support

Currently, we only support ext2 rev. 0.  We may add FAT support in future.

Virtual File System (VFS) is ported from OpenBSD to AIMv6 in order to enable
the system to work across multiple file systems smoothly.  We largely
simplified the implementation though.  Even so, the file system framework in
AIMv6 is *awfully* complicated as a result of planning to support multiple
file systems, including UFS-like file systems.

To aid study, I provide a framework description (TBD) and an end-to-end
walkthrough.

## Development constraints

Here are several constraints for reference if you are extending or modifying
the file system framework.  Breaking these rules likely calls for more or less
refactoring.

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

## Walkthrough

This walkthrough contains how the root file system is mounted from scratch,
how the file system interacts with a disk driver, how system calls are
translated into file system operations, etc., **step by step**.

I'm going to refer to symbols rather than source filenames and lines.
Although you can look up definitions and usages by `grep(1)` or `git grep`
command, building a cscope database will make your days easier.
Also, ~~real~~ some editors like
[Emacs](https://www.gnu.org/software/emacs/) and
[Vim](http://www.vim.org/) usually have built-in cscope support and/or
add-ons:

* [Cscope on Emacs](https://www.emacswiki.org/emacs/CScopeAndEmacs)
* [Cscope on Vim](http://cscope.sourceforge.net/cscope_vim_tutorial.html)

If your editor does not have cscope support, ~~throw your stupid editor away and
start learning Vim or Emacs~~ you can always try the interactive cscope
interface by running `cscope(1)` without arguments.

You can build a cscope database by running

```
find . -name '*.c' -or -name '*.h' -or -name '*.S' >cscope.files && cscope -bquk
```

