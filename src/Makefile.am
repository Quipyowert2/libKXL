lib_LTLIBRARIES = libKXL.la
libKXL_la_SOURCES = KXLmisc.c KXLsound.c KXLvisual.c KXLimage.c
include_HEADERS = KXL.h
libKXL_la_CFLAGS = -DUSE_PULSEAUDIO
libKXL_la_LDFLAGS = -lpulse-simple -release $(VERSION)
