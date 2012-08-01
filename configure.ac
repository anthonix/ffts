#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT(ffts, 0.7, amb@anthonix.com)
AM_INIT_AUTOMAKE(ffts, 0.7)

# AC_CONFIG_SRCDIR([include/common.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
#AX_COMPILER_VENDOR

#CXX="clang++"
#CXXFLAGS="$CXXFLAGS -stdlib=libc++"

#SFFT_AR="/usr/bin/ar"
#SFFT_CFLAGS="$CFLAGS"
#SFFT_CC="$CC"


AC_ARG_ENABLE(single, [AC_HELP_STRING([--enable-single],[compile single-precision library])], sfft_single=$enableval, sfft_single=no)
if test "$sfft_single" = "yes"; then
	AC_DEFINE(FFTS_PREC_SINGLE,1,[Define to FFT in single precision.])
fi
if test "$sfft_single" = "no"; then
	AC_DEFINE(FFTS_PREC_SINGLE,0,[Define to FFT in single precision.])
fi

#if test "$ord_sr" = "no"; then
#	AC_DEFINE(SFFT_ORD_SR,0,[Define to enable ordinary split radix.])
#fi

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gettimeofday pow])


AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile
                 ])
AC_OUTPUT
