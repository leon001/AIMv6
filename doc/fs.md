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

To aid study, I provided an end-to-end walkthrough here.

## Development constraints

Here are several constraints for reference if you are extending or modifying
the file system framework.  Breaking these rules likely calls for more or less
refactoring.

1. While OpenBSD has implemented reader-writer locks on vnodes, in AIMv6, we
  only have *exclusive* locks.
2. There is at most one vnode corresponding to each file on each file system.
  Moreover, there is at most one vnode corresponding to a device.
3. Any operation on a vnode requires the vnode to be locked, in order to
  prevent any possible race conditions and ensure serialization in a simple but
  brutal way.  This is accomplished via `vlock()` and `vunlock()` routines.
4. Any flag changes on a buf requires the interrupt to be disabled, in order
  to prevent race conditions where the flags are changed in an interrupt
  after reading the flag *AND* before doing anything with it.
  * Note that since the vnode of a buf is locked, other cores cannot do
    anything from outside.
5. The vnode and all of its contents will be sync'ed, cleaned up and reclaimed,
  as soon as its reference count drops to 0.

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

### Before bringing up file system

Because disk operations are slow compared to memory loads/stores, we usually want
to let the CPU do other stuff while the disk controller performs I/O.  Doing
this involves

1. Handling interrupts asserted by disk controller
2. Suspending a process (`sleep()` and `sleep_with_lock()`)
3. Resuming a process (`wakeup()`)

All of these imply that starting up a file system requires

* A working scheduler
* At least one running process

We put the job in a kernel process called `initproc`, which is the first
process being created (hence having a PID of 1).  The process itself is
created by function `spawn_initproc()`.

Spawning `initproc` is very easy:

* We allocate a process (`proc_new()`).
* We set it up as a kernel process (`proc_ksetup()`) and tell the kernel
  that the entry is at `initproc_entry()`.  Since the process is executing
  at kernel space for now, we do not need to create any user memory
  mapping.
* We mark the process as runnable.
* We add the process to the scheduler.

After enabling timer interrupt, all processors will try to suspend the
current process and pick the next one from scheduler.  For now, the
scheduler only holds one process `initproc`, so `initproc` will be
started, which means that `initproc_entry()` will be executed in the
context of a running process.

`initproc_entry()` calls `fsinit()`, which brings up the root file
system.

### Bringing up the file system

The overall steps of bringing up a file system includes:

* Locate a disk controller.
* Load disk partitions and determine which is the "root" partition.
* Determine which file system the root partition contains.
* Load the metadata of the file system into memory.

They are finished one-by-one in `mountroot()`.

#### `mountroot()`

`mountroot()` iterates over a list of function entry points
registered by file system providers, calling the function there
and check for the error code.  If the error code is 0, the
mounting is a success and `mountroot()` exits.

The function entry points are registered via `register_mountroot()`
function.  `register_mountroot()` is often called in file system
provider initialization code, which is executed in initcalls
before `fsinit()`.  The ext2 provider initialization routine is
called `ext2fs_register()`, which registers the ext2-specific
`mountroot()` implementation: `ext2fs_mountroot()`.

As we only provide one file system in current AIMv6 implementation,
`ext2fs_mountroot()` is the only one `mountroot()` operation to be
executed.

#### `ext2fs_mountroot()`

TBD
