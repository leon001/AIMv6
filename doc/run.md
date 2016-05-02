Running
------

We should probably automate these instructions.

### i386 under QEMU

#### Creating a blank disk image

```
$ dd if=/dev/zero of=disk.img bs=1M count=500
```

creates a 500M disk image.

#### Making partitions

```
$ fdisk disk.img
```

to interactively make partitions.

You can also use `sfdisk(8)` to manipulate disk partitions in a
script-oriented manner.

#### Mounting disk image partitions

You would probably have to run as root.

```
# kpartx -av disk.img
```

The partitions are mounted as `/dev/mapper/loop0pX` in Fedora, where
`X` corresponds to partition ID.

**TODO:** add running cases in Ubuntu etc.

#### Installing bootloader

```
$ dd if=boot/arch/i386/boot.bin of=disk.img conv=notrunc
```

Take extra care to ensure that `boot.bin` never exceeds 446 bytes, or
partition entries may be overwritten.

#### Installing kernel onto 2nd partition

On Fedora:

```
# dd if=kern/vmaim.elf of=/dev/mapper/loop0p2
```

**TODO:** add running cases in Ubuntu etc.

#### Running QEMU

```
$ qemu-system-i386 -serial mon:stdio -hda i386.img -smp 4 -m 512
```

#### Running QEMU and debug with GDB

```
$ qemu-system-i386 -serial mon:stdio -hda i386.img -smp 4 -m 512 \
> -S -gdb tcp::1234
```

Then run GDB and enter the following:

```
(gdb) target remote :1234
```

#### Debugging with GDB

If you are going to debug your kernel, you probably want to disable
optimization so that your C code matches the disassembly exactly.
To do so, you will have to reconfigure and rebuild the project with
a new `CFLAGS`:

```
$ env ARCH=i386 MACH=generic CFLAGS='-g -O0' ./configure \
> --enable-static --disable-shared --without-pic \
> --with-kern-start=0x100000 --with-mem-size=0x20000000
$ make clean
$ make
```

Step to next instruction:

```
(gdb) si
```

Switching layout:

```
(gdb) help layout
```

to see usage of `layout` command.  You can view disassembly/sources there.

Set breakpoint at address:

```
(gdb) b *0x7c00
```

to add a breakpoint at bootloader entry.

Loading debugging symbols:

```
(gdb) symbol kern/vmaim.elf
```

to enable C code debugging after QEMU stepped into kernel.

### ARM

### MIPS

#### Debugging

As of this writing, GDB remote debugging is not supported on MSIM simulator
and Loongson 3A.

