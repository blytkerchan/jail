#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(libconf, 0.0-alpha1, ronald@landheer.com)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([libconf.c])
AC_CONFIG_HEADER([config.h])

# Check the platform
AC_CANONICAL_HOST
case $host_os in
mingw32)
	posix_or_woe32="woe32" ;;
*)
	posix_or_woe32="posix" ;;
esac
AC_SUBST(POSIX_OR_WOE32, $posix_or_woe32)

# Checks for programs.
AC_PROG_YACC
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LEX
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdint.h stdlib.h string.h strings.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_VPRINTF
AC_REPLACE_FUNCS([memset strchr strdup])

AC_CONFIG_SUBDIRS([libcontain])
AC_CONFIG_FILES([Makefile
                 libreplace/Makefile
                 posix/Makefile
		 tests/Makefile
		 libconf_config.h])
AC_OUTPUT
