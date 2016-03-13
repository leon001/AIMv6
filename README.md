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

AIMv6 builds in UNIX-like environments, and have the following requirements:

* A proper toolchain. Cross-compiling toolchains works for all platforms. For
  IA32 targets, native toolchains are usable as well. See (TO BE FILLED) for
  details.

* A `sh`-compatible shell.

* `make`

Following steps should be taken to build AIMv6.

1. If this source tree comes from a source repository, run
   `autoreconf --install` here at the root of the source tree. If not, skip
   this step.

2. Run `./configure`, with parameters and environment variables according to
   your need.

3. Run `make`

AIMv6 does not come with any test suites right now.

### ARM

(TBD)

### MIPS

Currently two platforms are (planned to be) supported on MIPS architecture:

1. [MSIM](http://d3s.mff.cuni.cz/~holub/sw/msim/)
2. Loongson 3A (TBD)

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

### Loongson 3A

(TBD)

## Running AIMv6

See documents in (TO BE FILLED) for more details.

## Contributing to AIMv6

Read `doc/patterns.md` for code styles and design patterns adopted in our
project.  It is highly recommended to follow the guidelines there.
