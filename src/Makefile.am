

lib_LTLIBRARIES = libffts.la

libffts_la_SOURCES = cp_sse.c patterns.c codegen.c

libffts_includedir=$(includedir)/ffts
libffts_include_HEADERS = ../include/ffts.h

if HAVE_NEON
libffts_la_SOURCES += neon.s
else 
if HAVE_SSE
libffts_la_SOURCES += sse.s
endif
endif
