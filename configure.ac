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

# deal with platforms
# select what to compile for the firmware and the kernel
# and add corresponding macro defines
case $MACH in
	zynq)
		uart_zynq="primary"
		sd_zynq="primary"
	;;
esac
AM_CONDITIONAL([UART_ZYNQ], 
	[test x$uart_zynq = xprimary -o x$uart_zynq = xsecondary])
AM_CONDITIONAL([SD_ZYNQ], 
	[test x$sd_zynq = xprimary -o x$sd_zynq = xsecondary])

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

