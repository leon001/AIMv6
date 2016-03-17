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

nobase_noinst_HEADERS = \
	sys/types.h

# FIXME:
# I still think that it's better practice to put all arch-specific headers
# into a directory like 'asm', so that arch-specific code should include
# a header like <asm/asm.h>.
# The reason is that, if we do not put them into a directory, we are
# essentially forbidding same filenames for arch-specific header and
# general header.  E.g. if include/arch/mips/mm.h and include/mm.h exist,
# only one of them would be included, and you can't tell which would be
# included exactly.
# I renamed arch/mips/mm.h to arch/mips/arch-mm.h as a hotfix.
noinst_HEADERS = \
	bootsect.h \
	console.h \
	device.h \
	elf.h \
	file.h \
	init.h \
	mm.h \
	sleep.h \
	arch/armv7a/io.h \
	arch/armv7a/mach-zynq/mach.h \
	arch/mips/addrspace.h \
	arch/mips/asm.h \
	arch/mips/cp0regdef.h \
	arch/mips/io.h \
	arch/mips/irq.h \
	arch/mips/mipsregs.h \
	arch/mips/arch-mm.h \
	arch/mips/regdef.h \
	arch/mips/smp.h \
	arch/mips/mach-msim/mach-smp.h
