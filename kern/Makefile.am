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

include $(top_srcdir)/env.am

SUBDIRS = \
	arch \
	init

CLEANFILES = vmaim.s

noinst_PROGRAMS = vmaim.elf
noinst_DATA = vmaim.s

vmaim_elf_SOURCES =
vmaim_elf_LDADD = \
	arch/$(ARCH)/lib$(ARCH).la \
	arch/$(ARCH)/vmaim.lds \
	init/libinit.la \
	$(top_builddir)/drivers/libdrivers.la
# FIXME need better solution
# ld can find the script from input files
vmaim_elf_LDFLAGS = $(AM_LDFLAGS) -T arch/$(ARCH)/vmaim.lds

