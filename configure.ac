# Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
#
# This program is part of AIMv6.
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
LT_INIT
AC_CONFIG_MACRO_DIRS([m4])

# package information
AC_ARG_VAR([ARCH], [target architecture])
AC_SUBST([ARCH])
AC_ARG_VAR([MACH], [target machine])
AC_SUBST([MACH])
AC_ARG_VAR([BOARD], [target board])
AC_SUBST([BOARD])

# programs
AC_PROG_CC
AC_PROG_CC_STDC

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

AC_CONFIG_FILES([
	Makefile
	boot/Makefile
	boot/arch/Makefile
	boot/arch/armv7a/Makefile
	boot/arch/i386/Makefile
	boot/arch/mips/Makefile
	doc/Makefile
	drivers/Makefile
	firmware/Makefile
	firmware/arch/Makefile
	firmware/arch/armv7a/Makefile
	firmware/arch/i386/Makefile
	firmware/arch/mips/Makefile
	include/Makefile
	kern/Makefile
	tools/Makefile
	user/Makefile
])
AC_OUTPUT

