dnl Process this file with autoconf to produce a configure script.
AC_INIT([KXL], [1.1.5])
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

AC_OUTPUT([KXL.spec Makefile src/Makefile docs/Makefile])
