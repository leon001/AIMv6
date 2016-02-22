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

AC_PROG_CC
AC_PROG_CC_STDC
AC_C_CONST
AC_C_RESTRICT
AC_C_VOLATILE
AC_C_INLINE

AM_PROG_AS
AM_PROG_AR
AC_CHECK_TOOL([OBJDUMP], [objdump])
AC_CHECK_TOOL([OBJCOPY], [objcopy])

LT_INIT

# check for compiler attributes
AC_CONFIG_MACRO_DIR([./m4])
AIM_GCC_FUNC_ATTRIBUTE([noreturn])
AIM_GCC_FUNC_ATTRIBUTE([weak])

# package information
AC_ARG_VAR([ARCH], [target architecture])
AC_SUBST([ARCH])
AC_ARG_VAR([MACH], [target machine])
AC_SUBST([MACH])
AC_SUBST([PREFIXED_MACH], [mach-$MACH])
AC_ARG_VAR([MEM], [installed memory])
AC_SUBST([MEM])

# deal with architectures
case $ARCH in
	mips32)
		AC_DEFINE(USE_MIPS32, [], [MIPS32 ABI and instruction set])
	;;
	mips64)
		AC_DEFINE(USE_MIPS64, [], [MIPS64r2 ABI and instruction set])
	;;
	mips*)
		AS_VAR_SET([with_nr_cpus], [4])
esac

# deal with platforms
# select what to compile for the firmware and the kernel
# and add corresponding macro defines
case $MACH in
	zynq)
		enable_uart_zynq="yes"
		enable_sd_zynq="yes"
		enable_timer_a9="yes"
	;;
esac

# libraries
# none

# header files
# none

# types
# none

# structures
# none

# compiler characteristics
# none

# library functions
# none

# system services
# none

#AIM_SET_ARG([nr-cpus], [NR_CPUS], [Number of cores], [4])
AC_ARG_WITH(
	[nr-cpus], 
	[AS_HELP_STRING([nr-cpus], [Set number of cores])][]dnl
)
AS_IF(
	[test x$with_nr_cpus = x],
	[AS_VAR_SET([with_nr_cpus], [detect])]
)
AC_DEFINE([NR_CPUS], [$with_nr_cpus], [Number of cores])
AIM_SET_ARG([kstacksize], [KSTACKSIZE], [Size of kernel stack], [4096])

# option checks
AS_CASE($MACH,
	[msim], [AIM_SET_ARG([msim-kbd-pa],
			     [MSIM_KBD_PHYSADDR],
			     [MSIM keyboard physical address],
			     [0x1f000000])
		 AIM_SET_ARG([msim-lp-pa],
			     [MSIM_LP_PHYSADDR],
			     [MSIM printer physical address],
			     [0x1f000010])
		 AIM_SET_ARG([msim-disk-pa],
			     [MSIM_DISK_PHYSADDR],
			     [MSIM disk physical address],
			     [0x1f000020])
		 AIM_SET_ARG([msim-order-pa],
			     [MSIM_ORDER_PHYSADDR],
			     [MSIM IPC device physical address],
			     [0x1f000100])
		 AIM_SET_ARG([msim-mb-base],
			     [MSIM_ORDER_MAILBOX_BASE],
			     [MSIM IPC device mailbox base address],
			     [0x1f000200])
		 AIM_SET_ARG([msim-mb-order],
			     [MSIM_ORDER_MAILBOX_ORDER],
			     [order of MSIM IPC device mailbox (size=1<<order)],
			     [4])
		 AIM_SET_ARG([msim-rtc-pa],
			     [MSIM_RTC_PHYSADDR],
			     [MSIM real time clock physical address],
			     [0x1f000080])
		 AIM_SET_ARGSTR([msim-firmware-bin],
			     [MSIM_FIRMWARE_BIN],
			     [MSIM firmware binary file name],
			     ["firmware.bin"])
		 AIM_SET_ARGSTR([msim-disk-img],
			     [MSIM_DISK_IMG],
			     [MSIM disk image file name],
			     ["disk.img"])
		 AIM_SET_ARG([fwstack-order],
			     [FWSTACKORDER],
			     [order of firmware stack per CPU (size=1<<order)],
			     [12])
		 AIM_SUBST_MULTILINE(MSIM_CONF_CPULIST,
			  `for (( i=0; $i<$with_nr_cpus; i=$i+1 )); do \
			  	echo add dcpu cpu$i; \
			  done`)
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
			  `for (( i=0; $i<$with_nr_cpus; i=$i+1 )); do \
			   pa=$(( $with_msim_mb_base \
				+ ( $i << $with_msim_mb_order ) )); \
			   printf "add rwm mb%d 0x%08x\n" $i $pa; \
			   printf "mb%d generic %d\n" $i \
			   $(( 1 << $with_msim_mb_order )); \
			   done`)
		 AC_SUBST(MSIM_CONF_RTCPA,
			  `printf 0x%08x $with_msim_rtc_pa`)
		 ],
	[zynq], [],
	[AC_MSG_ERROR([No machine specified])])

# Drivers
AC_ARG_ENABLE(
	[uart-zynq], 
	[AIM_HELP_ENABLE([uart-zynq], [zynq UART driver])][]dnl
)
AS_IF(
	[test x$enable_uart_zynq != xyes],
	[AS_VAR_SET([enable_uart_zynq], [no])]
)
AM_CONDITIONAL([UART_ZYNQ],
	[test x$enable_uart_zynq = xyes])

AC_ARG_ENABLE(
	[sd-zynq], 
	[AIM_HELP_ENABLE([sd-zynq], [zynq SD driver])][]dnl
)
AS_IF(
	[test x$enable_sd_zynq != xyes],
	[AS_VAR_SET([enable_sd_zynq], [no])]
)
AM_CONDITIONAL([SD_ZYNQ],
	[test x$enable_sd_zynq = xyes])

AC_ARG_ENABLE(
	[timer-a9], 
	[AIM_HELP_ENABLE([timer-a9], [Cortex-A9 MPCore timer driver])][]dnl
)
AS_IF(
	[test x$enable_timer_a9 != xyes],
	[AS_VAR_SET([enable_timer_a9], [no])]
)
AM_CONDITIONAL([TIMER_A9],
	[test x$enable_timer_a9 = xyes])

# Primary Driver Selection

AM_CONDITIONAL([ARCH_ARMV7A],
	[test x$ARCH = xarmv7a])
AM_CONDITIONAL([ARCH_MIPS32],
	[test x$ARCH = xmips32])
AM_CONDITIONAL([ARCH_MIPS64],
	[test x$ARCH = xmips64])
AM_CONDITIONAL([UART_MSIM],
	[test x$MACH = xmsim])
AM_CONDITIONAL([BLOCK_MSIM],
	[test x$MACH = xmsim])
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
	kern/init/Makefile
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
  CFlags:	${CFLAGS}

Target
--------
  architecture:	${ARCH}
  machine:	${MACH}
  memory:	${MEM}
  processors:	${with_nr_cpus}

Drivers
--------
  uart-zynq:	${enable_uart_zynq}
  sd-zynq:	${enable_sd_zynq}
  timer-a9:	${enable_timer_a9}
])

