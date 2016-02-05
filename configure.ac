# Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
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

# package information
AC_ARG_VAR([ARCH], [target architecture])
AC_SUBST([ARCH])
AC_ARG_VAR([MACH], [target machine])
AC_SUBST([MACH])
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
esac

# deal with platforms
# select what to compile for the firmware and the kernel
# and add corresponding macro defines
case $MACH in
	zynq)
		uart_zynq="primary"
		sd_zynq="primary"
	;;
esac
AM_CONDITIONAL([ARCH_ARMV7A_LE],
	[test x$ARCH = xarmv7a-le])
AM_CONDITIONAL([ARCH_MIPS32],
	[test x$ARCH = xmips32])
AM_CONDITIONAL([UART_ZYNQ], 
	[test x$uart_zynq = xprimary -o x$uart_zynq = xsecondary])
AM_CONDITIONAL([SD_ZYNQ], 
	[test x$sd_zynq = xprimary -o x$sd_zynq = xsecondary])
AM_CONDITIONAL([UART_MSIM],
	[test x$MACH = xmsim])

# programs
AC_PROG_CC
AC_PROG_CC_STDC
AM_PROG_AS
AM_PROG_AR
AC_CHECK_TOOL([OBJDUMP], [objdump])
AC_CHECK_TOOL([OBJCOPY], [objcopy])


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
		 AIM_SET_ARG([msim-order-mb-base],
			     [MSIM_ORDER_MAILBOX_BASE],
			     [MSIM IPC device mailbox base address],
			     [0x1f000200])
		 AIM_SET_ARG([msim-order-mb-size],
			     [MSIM_ORDER_MAILBOX_SIZE],
			     [MSIM IPC device mailbox size],
			     [16])
		 AIM_SET_ARG([msim-order-rtc-pa],
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
			     ["disk.img"])],
	[AC_MSG_ERROR([No machine specified])])

LT_INIT

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([
	Makefile
	include/Makefile
	boot/Makefile
	boot/arch/Makefile
	boot/arch/armv7a/Makefile
	boot/arch/i386/Makefile
	boot/arch/mips/Makefile
	doc/Makefile
	drivers/Makefile
	drivers/sd/Makefile
	drivers/serial/Makefile
	firmware/Makefile
	firmware/arch/Makefile
	firmware/arch/armv7a/Makefile
	firmware/arch/i386/Makefile
	firmware/arch/mips/Makefile
	kern/Makefile
	tools/Makefile
	user/Makefile
])
AC_OUTPUT

