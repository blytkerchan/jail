# Makefile.am - process with automake and configure script..
# stuff we need
SUBDIRS=libreplace
EXTRA_DIST=Hash.7 README.in Hash.7.pod arch

# the library
lib_LTLIBRARIES = libcontain.la
libcontain_la_SOURCES = Hash.cc \
			StringHash.cc \
			IntHash.cc \
			c_interface.cc \
			NocaseStringHash.cc \
			IniHash.cc \
			arch/@ARCH@/compare_and_exchange.c \
			arch/@ARCH@/decrement.c \
			arch/@ARCH@/increment.c \
			stack.c \
			queue.c \
			list.c
pkginclude_HEADERS = 	Hash.h \
			StringHash.h \
			IntHash.h \
			IniHash.h \
			hash.h \
			NocaseStringHash.h \
			stack.h \
			queue.h \
			list.h
libcontain_la_LIBADD=libreplace/libreplace.la
AM_CFLAGS = -g -Wall -Werror -I$(top_srcdir)/arch/include
AM_CXXFLAGS = -g -Wall -Werror -I$(top_srcdir)/arch/include

# testsuite
noinst_PROGRAMS=test1 test2 test3 test4 test5 test6
TESTS = test1 test2 test3 test4 test5 test6
test1_SOURCES=test1.cc
test1_LDADD=libcontain.la
test2_SOURCES=test2.cc
test2_LDADD=libcontain.la
test3_SOURCES=test3.cc
test3_LDADD=libcontain.la
test4_SOURCES=test4.cc
test4_LDADD=libcontain.la
test5_SOURCES=test5.cc
test5_LDADD=libcontain.la
test6_SOURCES=test6.c
test6_LDADD=libcontain.la -lstdc++

# helper scripts for client programs
bin_SCRIPTS=config-libcontain

# documentation
man7MANS = Hash.7
noinst_DATA = README
%.7 : %.7.pod
	pod2man --section 7 --release "The Jail project" --center "libcontain documentation" $< > $@

%.3 : %.3.pod
	pod2man --section 3 --release "The Jail project" --center "libcontain documentation" $< > $@

README : Hash.7.pod README.in
	pod2text $< > $@
	cat $(srcdir)/README.in >> $@

