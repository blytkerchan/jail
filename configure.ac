AC_PREREQ(2.57)
AC_INIT([libmemory], [0.0-alpha1], [ronald@landheer.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([hptr.c])

# check the host system
AC_CANONICAL_HOST
source `dirname $0`/arch/config.arch
AC_SUBST(ARCH, ${host_cpu})

# checks for programs
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdint.h stdlib.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ALLOCA
#AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
