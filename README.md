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

### Loongson 3A

(TBD)

## Running AIMv6

See documents in (TO BE FILLED) for more details.

