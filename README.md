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

## Running AIMv6

See documents in (TO BE FILLED) for more details.

