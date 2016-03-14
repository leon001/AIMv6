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

# Add other submodule directories here
# HACK WARNING:
#     Apparently automake should build things in mach-generic subdirectory
#     first, then specific machine subdirectory.  Changing the order
#     would break the dependency and I have no idea how to automatically
#     resolve this issue by automake itself yet.
KERN_SUBDIRS = mach-generic $(PREFIXED_MACH)

SUBDIRS = $(KERN_SUBDIRS)
DIST_SUBDIRS = mach-generic mach-msim

noinst_LTLIBRARIES = libmips.la libentry.la
noinst_DATA = vmaim.lds

CLEANFILES = vmaim.lds

libentry_la_SOURCES = entry.S

libmips_la_SOURCES = arch_init.c
libmips_la_LIBADD = \
		    $(PREFIXED_MACH)/lib$(MACH).la

vmaim.lds: vmaim.lds.S
	$(COMPILE) -E -P $^ -o $@
