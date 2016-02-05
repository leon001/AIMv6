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

# AIM_SET_ARG([feature], [macro], [desc], [default-value])
AC_DEFUN([AIM_SET_ARG],
  [
    AC_MSG_CHECKING([$3])
    AC_ARG_WITH([$1], [AS_HELP_STRING([--with-$1=ARG],
    				      [Set $3 (default=$4)])],
		[],
		[with_]m4_bpatsubst([$1], -, _)=$4)
    AC_DEFINE_UNQUOTED([$2], [$with_]m4_bpatsubst([$1], -, _), [$3])
    AC_MSG_RESULT([$with_]m4_bpatsubst([$1], -, _))
  ]
)

# AIM_SET_ARGSTR([feature], [macro], [desc], [default-value])
AC_DEFUN([AIM_SET_ARGSTR],
  [
    AC_MSG_CHECKING([$3])
    AC_ARG_WITH([$1], [AS_HELP_STRING([--with-$1=ARG],
    				      [Set $3 (default=$4)])],
		[],
		[with_]m4_bpatsubst([$1], -, _)=$4)
    AC_DEFINE_UNQUOTED([$2], "[$with_]m4_bpatsubst([$1], -, _)", [$3])
    AC_MSG_RESULT([$with_]m4_bpatsubst([$1], -, _))
  ]
)
