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

AM_CFLAGS = -nostdinc -nostdlib

MOSTLYCLEANFILES = firmware.lds

noinst_PROGRAMS = firmware.elf

noinst_DATA = firmware.bin firmware.s

firmware_elf_SOURCES = fwasm.S fwmain.c
firmware_elf_LDADD = \
	firmware.lds \
	$(top_builddir)/drivers/libdrivers-raw.la
firmware_elf_LDFLAGS = -T firmware.lds

firmware.lds: firmware.lds.S
	$(COMPILE) -E -P $^ -o $@

SUFFIXES = .elf .bin .s
.elf.bin:
	$(OBJCOPY) -O binary -S $^ $@
.elf.s:
	$(OBJDUMP)  -S $^ > $@

