# Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
# Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
#
# This file is part of AIMv6.
#
# AIMv6 is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# AIMv6 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# autoconf requirements
AC_PREREQ([2.69])

AC_INIT([AIMv6], [0.1], [davidgao1001@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_CONFIG_MACRO_DIR([./m4])

# Disable the default -O2 which overrides the -Os for the bootloader
: ${CFLAGS="-g"}

AC_PROG_CC
# TODO: put these into acinclude.m4
AIM_PROG_CC_C11
AS_CASE([$aim_prog_cc_c11],
	[no], [AC_MSG_ERROR([Compiler does not support C11.])]
)
AC_C_CONST
AC_C_RESTRICT
AC_C_VOLATILE
AC_C_INLINE
AC_C_TYPEOF
AIM_GCC_FUNC_ATTRIBUTE([noreturn])
AIM_GCC_FUNC_ATTRIBUTE([weak])
AIM_GCC_FUNC_ATTRIBUTE([noinline])
AX_GCC_VAR_ATTRIBUTE([visibility])
AIM_REQUIRE_COMPILE_FLAG([-fplan9-extensions])

AC_SUBST([AIM_CFLAGS], [$aim_cflags])

AM_PROG_AS
AM_PROG_AR
AC_CHECK_TOOL([OBJDUMP], [objdump])
AC_CHECK_TOOL([OBJCOPY], [objcopy])

LT_INIT

# target machine
AIM_ARG_VAR([ARCH], [target architecture])
AIM_SUPPORT_ARCH([armv7a i386 mips32 mips64])

AIM_ARG_VAR([MACH], [target machine])
AC_SUBST([PREFIXED_MACH], [mach-$MACH])
AIM_SUPPORT_MACH([msim loongson3a zynq pc generic unknown])

# Default configurations
#
# When specifying whether and how a module is to be built, three values are
# possible:
# - no		The module will not be built. Headers will not be removed.
# - module	The module will be built into a seperate loadable object, and
#         	will not be accessible during early booting stages. Executables
#		outside the kernel, like the firmware and bootloader, will not
#		build nor access the module.
# - yes		The module will be built into the kernel, and initialized
#		according to defined entry(s). It cannot be unloaded from the
#		running kernel.
# If a module is compiled into the kernel and contains some functionality that
# exactly one instance is accessed during early booting stages, a "primary"
# variable can be set to its name. When this is done, the firmware and the
# bootloader will also access it.
# It is usually meaningless to compile a secondary implementation into the
# firmware or the bootloader, so implementations should check corresponding
# macros.
AS_VAR_SET([with_kstacksize], [4096])

AS_CASE([$ARCH],
	[arm*], [
		AS_VAR_SET([enable_io_mem], [yes])
		AS_VAR_SET([enable_io_port], [no])
	],
	[mips32], [
		AC_DEFINE(USE_MIPS32, [], [MIPS32 ABI and instruction set])
	],
	[mips64], [
		AC_DEFINE(USE_MIPS64, [], [MIPS64r2 ABI and instruction set])
	],
	[mips*], [
		AS_VAR_SET([enable_io_mem], [yes])
		AS_VAR_SET([enable_io_port], [no])
		AS_VAR_SET([with_cpus], [4])
	],
	[i386], [
		AS_VAR_SET([enable_io_mem], [yes])
		AS_VAR_SET([enable_io_port], [yes])
		AS_VAR_SET([enable_uart_ns16550], [yes])
		AS_VAR_SET([with_primary_console], [uart-ns16550])
	]
)

AS_CASE([$MACH],
	[zynq], [
		AS_VAR_SET([with_ram_physbase], [0x00100000])

		AS_VAR_SET([with_fwstack_order], [12])
		AS_VAR_SET([enable_uart_zynq], [yes])
		AS_VAR_SET([enable_sd_zynq], [yes])
		AS_VAR_SET([enable_timer_a9], [yes])

		AS_VAR_SET([with_primary_console], [uart-zynq])
		AS_VAR_SET([with_primary_storage], [sd-zynq])
		AS_VAR_SET([with_primary_timing], [timer-a9])
	],
	[msim], [
		AS_VAR_SET([with_fwstack_order], [12])
		AS_VAR_SET([enable_uart_msim], [yes])
		AS_VAR_SET([enable_block_msim], [yes])

		AS_VAR_SET([with_primary_console], [uart-msim])
		AS_VAR_SET([with_primary_storage], [block-msim])

		AS_VAR_SET([with_msim_kbd_pa], [0x1f000000])
		AS_VAR_SET([with_msim_lp_pa], [0x1f000010])
		AS_VAR_SET([with_msim_disk_pa], [0x1f000020])
		AS_VAR_SET([with_msim_order_pa], [0x1f000100])
		AS_VAR_SET([with_msim_mb_base], [0x1f000200])
		AS_VAR_SET([with_msim_mb_order], [4])
		AS_VAR_SET([with_msim_rtc_pa], [0x1f000080])
		AS_VAR_SET([with_msim_firmware_bin], ["firmware.bin"])
		AS_VAR_SET([with_msim_disk_img], ["disk.img"])
		AS_VAR_SET([with_cpus], [4])
	], 
	[pc], [
		AS_VAR_SET([with_fwstack_order], [12])
	],
	[loongson3a], [
		AS_VAR_SET([enable_uart_ns16550], [yes])

		AS_VAR_SET([with_primary_console], [uart-ns16550])

		AC_DEFINE([UART_BASE1], [0x1fe001e0],
			[Loongson 3A UART base address 1 - Do not change])
		AC_DEFINE([UART_BASE2], [0x1fe001e8],
			[Loongson 3A UART base address 2 - Do not change])
	],
	[qemu-generic], [],
	[generic], [],
	[unknown], [],
	[AC_MSG_ERROR([No machine specified])]
)

# msim
AIM_ARG_WITH([msim-kbd-pa], [MSIM_KBD_PHYSADDR],
	[MSIM keyboard physical address])
AIM_ARG_WITH([msim-lp-pa], [MSIM_LP_PHYSADDR],
	[MSIM printer physical address])
AIM_ARG_WITH([msim-disk-pa], [MSIM_DISK_PHYSADDR],
	[MSIM disk physical address])
AIM_ARG_WITH([msim-order-pa], [MSIM_ORDER_PHYSADDR],
	[MSIM IPC device physical address])
AIM_ARG_WITH([msim-mb-base], [MSIM_ORDER_MAILBOX_BASE],
	[MSIM IPC device mailbox base address])
AIM_ARG_WITH([msim-mb-order], [MSIM_ORDER_MAILBOX_ORDER],
	[order of MSIM IPC device mailbox (size=1<<order)])
AIM_ARG_WITH([msim-rtc-pa], [MSIM_RTC_PHYSADDR],
	[MSIM real time clock physical address])
AIM_ARG_WITH([msim-firmware-bin], [MSIM_FIRMWARE_BIN],
	[MSIM firmware binary file name])
AIM_ARG_WITH([msim-disk-img], [MSIM_DISK_IMG],
	[MSIM disk image file name])

# target details
AIM_ARG_WITH([ram-physbase], [RAM_PHYSBASE], [physical memory start point],
	[0x00000000])
AC_SUBST([RAM_PHYSBASE], [$with_ram_physbase])

AIM_ARG_WITH([kern-start], [KERN_START], [kernel loading physical address],
	[0x00000000])
AC_SUBST([KERN_START], [$with_kern_start])

AIM_ARG_WITH([mem-size], [MEM_SIZE], [amount of memory installed], [detect])
AC_SUBST([MEM_SIZE], [$with_mem_size])

AIM_ARG_WITH([cpus], [NR_CPUS], [number of cores], [detect])
AC_SUBST([NR_CPUS], [$with_cpus])

AIM_ARG_WITH([kern-base], [KERN_BASE], [start of kernel address], [0x80000000])
AC_SUBST([KERN_BASE], [$with_kern_base])

AIM_ARG_WITH([kmmap-base], [KMMAP_BASE], [start of kernel dynamic mapping],
	[0xf0000000])
AC_SUBST([KMMAP_BASE], [$with_kmmap_base])

AIM_ARG_WITH([reserved-base], [RESERVED_BASE], [start of reserved high address],
	[0xfffff000])
AC_SUBST([RESERVED_BASE], [$with_reserved_base])

AIM_ARG_WITH([kstacksize], [KSTACKSIZE],
	[size of kernel stack])

AIM_ARG_WITH([fwstack-order], [FWSTACKORDER],
	[order of firmware stack per CPU (size=1<<order)])

# Drivers
AIM_ARG_ENABLE([io-mem], [IO_MEM], [memory-mapped IO driver])
AIM_ARG_ENABLE([io-port], [IO_PORT], [port IO driver])
AIM_ARG_ENABLE([uart-zynq], [UART_ZYNQ], [zynq UART driver])
AIM_ARG_ENABLE([uart-msim], [UART_MSIM], [MSIM UART driver])
AIM_ARG_ENABLE([uart-ns16550], [UART_NS16550], [NS16550 UART driver])
AIM_ARG_ENABLE([block-msim], [BLOCK_MSIM], [MSIM block device driver])
AIM_ARG_ENABLE([sd-zynq], [SD_ZYNQ], [zynq SD driver])
AIM_ARG_ENABLE([timer-a9], [TIMER_A9], [Cortex-A9 MPCore timer driver])

# Primary Driver Selection
AIM_ARG_WITH([primary-console], [PRIMARY_CONSOLE], [primary console])
AIM_ARG_WITH([primary-storage], [PRIMARY_STORAGE], [primary storage])
AIM_ARG_WITH([primary-timing], [PRIMARY_TIMING], [primary timing])

AS_CASE([$with_primary_console],
	[uart-zynq],[
		AC_DEFINE_UNQUOTED([CONSOLE_NEED_CR], [], [Primary console needs CR])
	]
)

# Algorithms
# MUST have unique names.
# First fit is named "ff" as page allocator, "flff" as simple allocator.
AS_VAR_SET([with_simple_allocator], [flff])
AIM_ARG_WITH([simple-allocator], [SIMPLE_ALLOCATOR], [non-caching memory object allocator])
AM_CONDITIONAL([ALGO_FLFF], [test x$with_simple_allocator = xflff])

AS_VAR_SET([with_page_allocator], [ff])
AIM_ARG_WITH([page-allocator], [PAGE_ALLOCATOR], [page allocator])
AM_CONDITIONAL([ALGO_FF], [test x$with_page_allocator = xff])

# per-target configuration
AS_CASE([$MACH],
	[msim], [
		 AIM_SUBST_MULTILINE([MSIM_CONF_CPULIST],
		 	`for (( i=0; $i<$with_cpus; i=$i+1 )); do \
			echo add dcpu cpu$i; \
			done`
		 )
		 AC_SUBST(MSIM_CONF_FIRMWARE_BIN, $with_msim_firmware_bin)
		 AC_SUBST(MSIM_CONF_KBDPA,
			  `printf 0x%08x $with_msim_kbd_pa`)
		 AC_SUBST(MSIM_CONF_LPPA,
			  `printf 0x%08x $with_msim_lp_pa`)
		 AC_SUBST(MSIM_CONF_DISKPA,
			  `printf 0x%08x $with_msim_disk_pa`)
		 AC_SUBST(MSIM_CONF_DISK_IMAGE, $with_msim_disk_img)
		 AC_SUBST(MSIM_CONF_ORDERPA,
			  `printf 0x%08x $with_msim_order_pa`)
		 AIM_SUBST_MULTILINE(MSIM_CONF_MBLIST,
			  `for (( i=0; $i<$with_cpus; i=$i+1 )); do \
			   pa=$(( $with_msim_mb_base \
				+ ( $i << $with_msim_mb_order ) )); \
			   printf "add rwm mb%d 0x%08x\n" $i $pa; \
			   printf "mb%d generic %d\n" $i \
			   $(( 1 << $with_msim_mb_order )); \
			   done`)
		 AC_SUBST(MSIM_CONF_RTCPA,
			  `printf 0x%08x $with_msim_rtc_pa`)
])

# Skip firmware
AM_CONDITIONAL([NO_FIRMWARE],
	[test x$MACH = xloongson3a -o x$ARCH = xi386])
# Skip bootloader
AM_CONDITIONAL([NO_BOOTLOADER],
	[test x$MACH = xloongson3a])

# change mips32 and mips64 to mips afterwards, because the CFLAGS are
# different, but the source files are in the same directory.
if test x$ARCH = xmips32 -o x$ARCH = xmips64; then
	ARCH=mips
fi

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([
	msim.conf
	Makefile
	include/Makefile
	include/aim/Makefile
	include/asm-generic/Makefile
	boot/Makefile
	boot/arch/Makefile
	boot/arch/armv7a/Makefile
	boot/arch/i386/Makefile
	boot/arch/mips/Makefile
	boot/arch/mips/mach-msim/Makefile
	doc/Makefile
	drivers/Makefile
	drivers/block/Makefile
	drivers/io/Makefile
	drivers/sd/Makefile
	drivers/serial/Makefile
	drivers/timer/Makefile
	firmware/Makefile
	firmware/arch/Makefile
	firmware/arch/armv7a/Makefile
	firmware/arch/i386/Makefile
	firmware/arch/mips/Makefile
	firmware/arch/mips/mach-msim/Makefile
	kern/Makefile
	kern/arch/Makefile
	kern/arch/armv7a/Makefile
	kern/arch/armv7a/mach-zynq/Makefile
	kern/arch/i386/Makefile
	kern/arch/mips/Makefile
	kern/arch/mips/mach-generic/Makefile
	kern/arch/mips/mach-msim/Makefile
	kern/arch/mips/mach-loongson3a/Makefile
	kern/init/Makefile
	kern/dev/Makefile
	kern/debug/Makefile
	kern/mm/Makefile
	kern/mm/pmm/Makefile
	kern/mm/vmm/Makefile
	kern/proc/Makefile
	lib/Makefile
	lib/libc/Makefile
	lib/libc/arch/Makefile
	lib/libc/arch/armv7a/Makefile
	lib/libc/arch/i386/Makefile
	lib/libc/arch/mips/Makefile
	tools/Makefile
	user/Makefile
])
AC_OUTPUT

AC_MSG_NOTICE([
=========================
  Configuration Summary
=========================

General
--------
  version:	${VERSION}
  build:	${build}
  host:		${host}
  compiler:	${CC}
  CPPFLAGS:	${CPPFLAGS}
  CFLAGS:	${CFLAGS}
  LDFLAGS:	${LDFLAGS}

Target
--------
  architecture:	${ARCH}
  machine:	${MACH}
  processors:	${NR_CPUS}
  memory:	${MEM_SIZE}

Kernel
--------
  KERN_BASE:	${with_kern_base}
  KMMAP_BASE:	${with_kmmap_base}
  RESERVED:	${with_reserved_base}

Drivers
--------
  io-mem:	${enable_io_mem}
  io-port:	${enable_io_port}
  uart-zynq:	${enable_uart_zynq}
  uart-msim:	${enable_uart_msim}
  sd-zynq:	${enable_sd_zynq}
  timer-a9:	${enable_timer_a9}

Primary Drivers
--------
  console:	${with_primary_console}
  storage:	${with_primary_storage}
  timing:	${with_primary_timing}

Firmware
--------
  stack order:	${with_fwstack_order}
])

