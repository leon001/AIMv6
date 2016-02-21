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

include $(top_srcdir)/env.am

noinst_PROGRAMS = boot.elf
noinst_DATA = boot.bin boot.s

CLEANFILES = boot.bin boot.s

boot_elf_SOURCES = bootsect.c bootsect.ld
boot_elf_CPPFLAGS = $(AM_CPPFLAGS) -DRAW
boot_elf_CFLAGS = $(AM_CFLAGS) -Os
boot_elf_LDFLAGS = $(AM_LDFLAGS) -T bootsect.ld -nostdlib

boot.bin: boot.elf
	$(OBJCOPY) -O binary -j.text $^ $@
