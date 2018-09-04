dnl Configure Paths for KXL
dnl AM_PATH_KXL
dnl Test for libKXL, and define KXL_CFLAGS and KXL_LIBS as appropriate.
dnl enables arguments --with-kxl-prefix= -with-kxl-inc-prefix=
dnl
AC_DEFUN(AM_PATH_KXL,
[dnl
dnl Get the cfags and libraries for KXL
dnl
AC_ARG_WITH(kxl-prefix, [  --with-kxl-prefix=PFX   Prefix where KXL library is installed(optional)],
	[kxl_prefix="$withval"], [kxl_prefix=""])
AC_ARG_WITH(kxl-inc-prefix, [  --with-kxl-inc-prefix=PFX Prefix where include libraries are (optional)],
	[kxl_inc_prefix="$withval"], [kxl_inc_prefix=""])

dnl Add any special include directories
AC_MSG_CHECKING(for KXL CFLAGS)
if test "$kxl_inc_prefix" != "" ; then
	KXL_CFLAGS="$KXL_CFLAGS -I$kxl_inc_prefix"
	CFLAGS="-I$kxl_inc_prefix"
fi
AC_MSG_RESULT($KXL_CFLAGS)

dnl add any special lib dirs
AC_MSG_CHECKING(for KXL LDFLAGS)
if test "$kxl_prefix" != "" ; then
	KXL_LIBS="$KXL_LIBS -L$kxl_prefix"
	LIBS="-L$kxl_prefix"
fi

dnl add the KXL library
KXL_LIBS="$KXL_LIBS -lKXL"
LDFLAGS="$KXL_LIBS"
AC_MSG_RESULT($KXL_LIBS)

AC_PATH_XTRA
AC_CHECK_LIB([KXL], [KXL_CreateWindow],,
        [AC_MSG_ERROR(No linkable libKXL was found.)], [$X_LIBS -lX11]
)

dnl That should be it.  Now just export out symbols:
AC_SUBST(KXL_CFLAGS)
AC_SUBST(KXL_LIBS)
])
