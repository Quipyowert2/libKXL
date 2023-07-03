dnl Process this file with autoconf to produce a configure script.
AC_INIT([KXL], [1.2.4])
AC_CONFIG_SRCDIR([src/KXL.h])
AM_INIT_AUTOMAKE
dnl Checks for programs.
AC_PROG_CC
AM_PROG_LIBTOOL
dnl Checks for libraries.

dnl Checks for header files.
AC_PATH_XTRA
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h malloc.h sys/ioctl.h sys/time.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_TIME

dnl Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_TYPE_SIGNAL
AC_CHECK_FUNCS(select)

aclocaldir='${datadir}/aclocal'
AC_SUBST(aclocaldir)

AC_ARG_ENABLE([test],
  [AS_HELP_STRING([--enable-test], [enable fuzz testing KXL_LoadBitmap
  @<:@default=no@:>@])],
  [test=$enableval], [test=no])

if test x"$test" = xyes; then
  AC_DEFINE([TEST], [1], [Define if building fuzz program])
  AC_SUBST([TEST])
  AC_CONFIG_FILES([test/Makefile])
fi

AM_CONDITIONAL([ENABLE_TEST], [test x"$test" = xyes])

AC_OUTPUT([KXL.spec Makefile src/Makefile docs/Makefile])