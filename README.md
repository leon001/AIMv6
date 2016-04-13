# AIMv6

AIMv6 project builds an operating system for teaching purposes.
This system should seperate kernel logic from platform-dependent implementation,
thus should work across platforms.

## Supported platforms

* ARMv7A
  - Xilinx Zynq-7000 SoC (2x Cortex A9)
* IA32
  - (TO BE FILLED)
* MIPS
  - (TO BE FILLED)

## Building AIMv6

### Prerequisites

AIMv6 builds in UNIX-like environments, and have the following requirements:

* A proper toolchain. Cross-compiling toolchains works for all platforms. For
  IA32 targets, native toolchains are usable as well. See `doc/toolchain.md`
  for details.

  - The C compiler **MUST** support C11 standard.

  - The C compiler **MUST** support plan9 extensions.

* A `sh`-compatible shell.

* `make`

* A full set of GNU autotools if building in maintainer mode (when this source
  tree comes from a source repository instead of a tarball).

  - autoconf **MUST** provide the macro `AC_PROG_CC_C11`, thus must come from
    the repository directly, later than the commit `db36f6d`. Grabbing the
    latest source from the master branch is recommended.

  - autoconf-archive must be present. It can be installed from your
    distribution's software repository.

### Normal mode

If you receive this source tree in the form of a tarball, or if the `configure`
script is present, AIMv6 can be built in normal mode.

Following steps should be taken to build AIMv6.

1. Run `./configure`, with parameters and environment variables according to
   your need.

2. Run `make`

AIMv6 does not come with any test suites right now.

AIMv6 can install built object under given prefix, but this feature is not
finished yet.

### Maintainer mode

If AIMv6 cannot be built in normal mode, a lot of generated scripts are not
present yet. To keep the reporsitory clean, generated files **SHOULD NOT** be
included in commits. Before you can build the source, prepare the source tree:

1. Grab `autoconf` source from [git://git.sv.gnu.org/autoconf.git].

2. Run `autoreconf --install` at the root of the `autoconf` source. Note that
   you are building `autoconf` in maintainer mode as well, so a working set
   of GNU autotools is required. The `autoconf` binary version requirement
   is 2.60.

3. Configure and install `autoconf`. Installation under `$HOME` is highly
   recommended. Do not overwrite the installed version unless you are on LFS
   **AND YOU ABSOLUTELY KNOW WHAT YOU ARE DOING**. AIMv6 team is not
   responsible for any possible damage to your system. If you are building
   a custom toolchain, you **SHOULD** choose a different prefix for `autoconf`.

4. Add the `bin` folder under your installation prefix **BEFORE** your `$PATH`
   variable. You may want to add this to your `~/.profile`. You should limit
   this effect to the unprivileged user you are currently using, as this
   version of `autoconf` is highly experimental. Also, make sure there's no
   other programs (not included in the `autoconf` install) under the same
   directory, unless you clearly know what you are doing and really mean it.

### MIPS

Currently two platforms are supported on MIPS architecture:

1. [MSIM](http://d3s.mff.cuni.cz/~holub/sw/msim/)
2. Loongson 3A

#### MSIM configuration

A suggested configuration is

```
./configure --host=mips64el-n64-linux-uclibc \
            --without-pic \
            --enable-static \
            --disable-shared
```

Replace `mips64el-n64-linux-uclibc` to the prefix of any available toolchain.
For example, if you have a MIPS GCC compiler named
`mips64-unknown-linux-gnu-gcc`, the host argument should be
`mips64-unknown-linux-gnu`.

##### Preparing disk image

MSIM needs a disk image to simulate hard disks.

###### A sample configuration

**May be changed later**.

Run

```
dd if=/dev/zero of=disk.img bs=1024 count=1000000
```

to create a 1000000K blank disk image.

Then, use `fdisk(8)` to make DOS partitions:

```
fdisk disk.img
```

Copy the bootloader into the first 446 bytes in the first sector:

```
dd if=boot/arch/mips/msim/boot.bin of=disk.img bs=1 count=446 conv=notrunc
```

Use `kpartx(8)` to mount the partitions in disk image (probably as root):

```
kpartx -av disk.img
```

If successful, you may see the following messages:

```
add map loop0p1 (253:3): 0 204800 linear /dev/loop0 2048
add map loop0p2 (253:4): 0 40960 linear /dev/loop0 206848
add map loop0p3 (253:5): 0 1024000 linear /dev/loop0 247808
add map loop0p4 (253:6): 0 776768 linear /dev/loop0 1271808
```

Copy the kernel image to the second partition on disk image (probably as root):

```
dd if=kern/arch/mips/kernel.elf of=/dev/mapper/loop0p2 conv=notrunc
```

Run MSIM to bring up the operating system.

#### Loongson 3A

You should prepare a SATA-to-USB converter or something to enable you to upload
your kernel image into the hard disk inside Loongson 3A box.

Replace the `boot/vmlinux` file with the kernel image you compiled, and you're
done.

## Running AIMv6

### ARM on qemu

Below is an example on how to test AIMv6: (A lot of work needed here)

```sh
qemu-system-arm -M xilinx-zynq-a9 -m 512 -serial null -serial stdio \
	-kernel firmware/arch/armv7a/firmware.elf -s -S
```

The `-s -S` option will enable gdb support on port 1234. You can then run
`gdb firmware/arch/armv7a/firmware.elf` with needed options and connect to the
emulator via `target remote:1234` in its command line.

## Contributing to AIMv6

Read `doc/patterns.md` for code styles and design patterns adopted in our
project.  It is highly recommended to follow the guidelines there.

