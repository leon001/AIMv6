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

noinst_PROGRAMS = firmware.elf
noinst_DATA = firmware.bin firmware.s

CLEANFILES = firmware.bin firmware.s

firmware_elf_SOURCES = entry.S main.c firmware.ld
firmware_elf_LDADD = $(top_builddir)/drivers/libdrivers-raw.la
firmware_elf_CPPFLAGS = $(AM_CPPFLAGS_NOPIC) -DRAW
firmware_elf_CFLAGS = $(AM_CFLAGS_NOPIC)
firmware_elf_CCASFLAGS = $(AM_CCASFLAGS_NOPIC)
firmware_elf_LDFLAGS = $(AM_LDFLAGS_NOPIC) -T firmware.ld

firmware.bin: firmware.elf
	$(OBJCOPY) -O binary -j.text -j.rodata -j.data -j.bss $^ $@
