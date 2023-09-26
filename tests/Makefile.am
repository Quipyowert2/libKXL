TESTS = check_foo
check_PROGRAMS = check_foo
check_foo_SOURCES = test_001.c $(top_builddir)/src/KXL.h $(top_builddir)/src/KXLimage.c \
    $(top_builddir)/src/KXLvisual.c $(top_builddir)/src/KXLsound.c $(top_builddir)/src/KXLmisc.c
check_foo_CFLAGS = @CHECK_CFLAGS@ --coverage -DUSE_PULSEAUDIO -I$(top_builddir)/src/
check_foo_LDADD = @CHECK_LIBS@ -lX11 -lpulse-simple -lpulse
check_foo_LDFLAGS = --coverage