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
# none

AC_INIT([AIMv6], [0.1], [davidgao1001@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_CONFIG_MACRO_DIR([./m4])

AC_PROG_CC
AC_PROG_CC_STDC
AC_C_CONST
AC_C_RESTRICT
AC_C_VOLATILE
AC_C_INLINE
AC_C_TYPEOF
AIM_GCC_FUNC_ATTRIBUTE([noreturn])
AIM_GCC_FUNC_ATTRIBUTE([weak])

AM_PROG_AS
AM_PROG_AR
AC_CHECK_TOOL([OBJDUMP], [objdump])
AC_CHECK_TOOL([OBJCOPY], [objcopy])

LT_INIT

# target machine
AIM_ARG_VAR([ARCH], [target architecture])

AIM_ARG_VAR([MACH], [target machine])
AC_SUBST([PREFIXED_MACH], [mach-$MACH])

# default configurations
AS_VAR_SET([with_kstacksize], [4096])

AS_CASE([$ARCH], 
	[mips32], [
		AC_DEFINE(USE_MIPS32, [], [MIPS32 ABI and instruction set])
	],
	[mips64], [
		AC_DEFINE(USE_MIPS64, [], [MIPS64r2 ABI and instruction set])
	],
	[mips*], [
		AS_VAR_SET([with_cpus], [4])
	]
)

AS_CASE([$MACH],
	[zynq], [
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
		AS_VAR_SET([with_primary_console], [uart-msim])
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
	],
	[loongson3a], [
		# you don't really have any option here.
		AS_VAR_SET([enable_uart_ns16550], [yes])
		AC_DEFINE([UART_BASE1], [0x1fe001e0],
			[Loongson 3A UART base address 1 - Do not change])
		AC_DEFINE([UART_BASE2], [0x1fe001e8],
			[Loongson 3A UART base address 2 - Do not change])
	],
	[AC_MSG_ERROR([No machine specified])]
)

# target details
AIM_ARG_WITH([mem-size], [MEM_SIZE], [amount of memory installed], [detect])
AC_SUBST([MEM_SIZE], [$with_mem_size])

AIM_ARG_WITH([cpus], [NR_CPUS], [number of cores], [detect])
AC_SUBST([NR_CPUS], [$with_cpus])

AIM_ARG_WITH([kstacksize], [KSTACKSIZE],
	[size of kernel stack])

AIM_ARG_WITH([fwstack-order], [FWSTACKORDER],
	[order of firmware stack per CPU (size=1<<order)])

# Drivers
AIM_ARG_ENABLE([uart-zynq], [UART_ZYNQ], [zynq UART driver])
AIM_ARG_ENABLE([uart-msim], [UART_MSIM], [MSIM UART driver])
AIM_ARG_ENABLE([uart-ns16550], [UART_NS16550], [NS16550 UART driver])
AIM_ARG_ENABLE([sd-zynq], [SD_ZYNQ], [zynq SD driver])
AIM_ARG_ENABLE([timer-a9], [TIMER_A9], [Cortex-A9 MPCore timer driver])

# Primary Driver Selection
AIM_ARG_WITH([primary-console], [PRIMARY_CONSOLE], [primary console])
AIM_ARG_WITH([primary-storage], [PRIMARY_STORAGE], [primary storage])
AIM_ARG_WITH([primary-timing], [PRIMARY_TIMING], [primary timing])

# per-target configuration
AS_CASE($MACH,
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

AIM_SUPPORT_ARCH([armv7a i386 mips32 mips64])

AM_CONDITIONAL([UART_MSIM],
	[test x$MACH = xmsim])
# FIXME: probably should relate to --enable-uart-ns16550 option?
AM_CONDITIONAL([UART_NS16550],
	[test x$MACH = xloongson3a])
AM_CONDITIONAL([BLOCK_MSIM],
	[test x$MACH = xmsim])

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
	boot/Makefile
	boot/arch/Makefile
	boot/arch/armv7a/Makefile
	boot/arch/i386/Makefile
	boot/arch/mips/Makefile
	boot/arch/mips/mach-msim/Makefile
	doc/Makefile
	drivers/Makefile
	drivers/block/Makefile
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
	lib/Makefile
	lib/libc/Makefile
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

Drivers
--------
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

