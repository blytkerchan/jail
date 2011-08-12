# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.57)
AC_INIT([jail-ust-preproc],[0.0-alpha1],[ronald@landheer.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([preprocl.l])
AC_CONFIG_HEADER([config.h])

# check the host system
AC_CANONICAL_HOST
source `dirname $0`/arch/config.arch
AC_SUBST(ARCH, ${host_cpu})

# Checks for programs.
AC_PROG_CC
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_LIBTOOL 

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_REPLACE_FUNCS([strlwr])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_SUBDIRS(libcontain libmemory)
AC_OUTPUT

