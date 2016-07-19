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

### Development constraints

**To readers of walkthrough: if you are not developers improving, enhancing,
or refactoring this file system framework, please skip this section.  Developers
who are unfamiliar with the framework in BSD are also encouraged to read the
walkthrough below first, then jump back here.**

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

`ext2fs_mountroot()` is the function which provides the `mountroot()`
implementation on an ext2 file system, which, along with its
descendants (including ext3 and ext4), is widely used in modern
Unix-like systems such as Linux and BSD.

The technical details of ext2 can be found
[here](http://wiki.osdev.org/Ext2).

`ext2fs_mountroot()` consists of four operations:

1. Obtaining a kernel representation of the device holding the root
  file system via the following call:

  ```C
  bdevvp(rootdev, &rootvp)
  ```

2. Create an object representing the file system as a whole, assigning
  file-system-wide operations to it as methods:

  ```C
  vfs_rootmountalloc("ext2fs", &mp)
  ```
  
3. Load the metadata of the file system into memory:

  ```C
  ext2fs_mountfs(rootvp, mp, current_proc)
  ```
  
4. Add the file system object `mp` into the list of loaded file systems.
  This operation is quite trivial by itself and I will not extend it
  further:

  ```C
  addmount(mp)
  ```
  
### Vnode - the kernel in-memory representation of a file

The `bdevvp()` call takes a device identifier (with type `dev_t`) as
input, and produces a `struct vnode` object as output.  This `struct
vnode` object, or *vnode* for short, represents the device we are going
to read and write on.  In this section, I'll explain what is a vnode,
before moving on to `bdevvp()`, which will be trivial to understand
then.

Vnode is the kernel representation of a file in AIMv6 (and also BSD
and Linux - though Linux uses the term "inode", a UFS-specific term,
across all file systems for this purpose).

#### Files, directories, and device files

A file, by [definition](https://en.wikipedia.org/wiki/Computer_file), is
a kind of information storage, available for programs to read and write
as a unit.

A [directory](https://en.wikipedia.org/wiki/Directory_(computing)) is
either a collection of files, or a collection of file references.  In
Unix, directories are treated as files, so they share the same
kernel representation.

Unix (and also Windows!) further extended the concept of files by
treating devices as *special files*, and device accesses and manipulations
as file operations.  For example, outputting text to a serial console
is viewed as `write`'s, while receiving information from serial console
are treated as `read`'s.  The operations which are rather hard to
classify as ordinary file operations (such as changing baud rate of
a serial console) are called `ioctl`'s.

Occasionally, inter-process communications, including pipes and sockets,
can also appear in various forms of file operations.

A file does not need to be actually persist in durable storages such
as hard disks; it can be only present in memory, sometimes usable only
by processes requested to open it (e.g. pipes and sockets).

There are more concepts and objects that can be treated as files, some of
which are even beyond imagination ~~and are proposed by dark magic
practitioners from Plan 9~~:

* Special data sources and sinks (e.g. random number generator)
* Memory
* CPU
* Web servers
* Wikipedia
* [~~Your belongings~~](https://en.wikipedia.org/wiki/Dunnet_(video_game))
* 

#### Vnodes (Part 1)

A vnode has a lot of members, but for now we only need to focus on some
of them:

* `type` - One of the following:

  |Type   |Description (Unix)    |Windows equivalent     |
  |-------|----------------------|-----------------------|
  |`VREG` |Regular file          |Good ol' everyday files|
  |`VDIR` |Directory             |Folders                |
  |`VCHR` |Character device file |                       |
  |`VBLK` |Block device file     |                       |
  |`VLNK` |Symbolic link         |Shortcuts              |
  
  And possibly one of the following, which are not available in AIMv6 but in
  BSD systems:
  
  |Type   |Description (Unix)    |Windows equivalent     |
  |-------|----------------------|-----------------------|
  |`VFIFO`|Named pipes           |                       |
  |`VSOCK`|Sockets               |                       |

* `ops` - The underlying implementation of file operations, usually assigned
  by file system providers upon creation, and depends on the file system and
  file type.  For example, reading from a regular file is certainly different
  from reading from a device.
* `mount` - The file system object the vnode resides on.
* `typedata` - Depending on vnode type, this may point to different structures.
  For now, we only consider member `specinfo`, which points to a `specinfo`
  structure if the vnode corresponds to a device (either `VCHR` or `VBLK`).


#### `getdevvp()`, `bdevvp()` and `cdevvp()`

