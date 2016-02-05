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
