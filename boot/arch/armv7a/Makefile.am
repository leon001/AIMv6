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

dist_noinst_DATA = boot.lds.S

noinst_PROGRAMS = bootloader.elf
noinst_DATA = bootloader.bin bootloader.s

CLEANFILES = bootloader.bin bootloader.s boot.lds

bootloader_elf_SOURCES = bootmain.c
bootloader_elf_CPPFLAGS = $(AM_CPPFLAGS) -DRAW
bootloader_elf_CFLAGS = $(AM_CFLAGS) -Os
bootloader_elf_DEPENDENCIES = boot.lds
bootloader_elf_LDFLAGS = $(AM_LDFLAGS) -T boot.lds

boot.lds: boot.lds.S
	$(COMPILE) -E -P $^ -o $@

