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

AM_CPPFLAGS = \
	-I$(top_srcdir)/include/arch/$(ARCH) \
	-I$(top_srcdir)/include

noinst_LTLIBRARIES = libserial-raw.la

libserial_raw_la_SOURCES = uart.c uart.h
libserial_raw_la_CPPFLAGS = $(AM_CPPFLAGS) -DRAW

if UART_ZYNQ
libserial_raw_la_SOURCES += uart-zynq.c uart-zynq.h
else
libserial_raw_la_CPPFLAGS += -DUART_ZYNQ
endif

