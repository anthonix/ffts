AUTOMAKE_OPTIONS = foreign
SUBDIRS = src tests 
EXTRA_DIST=COPYRIGHT ffts.pc.in
ACLOCAL_AMFLAGS = -Im4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ffts.pc

if ENABLE_JNI
SUBDIRS += java
endif
