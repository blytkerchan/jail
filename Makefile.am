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
			arch/@ARCH@/compare_and_exchange.c \
			arch/@ARCH@/decrement.c \
			arch/@ARCH@/increment.c \
			stack.c \
			queue.c \
			list.c
pkginclude_HEADERS = 	Hash.h \
			StringHash.h \
			IntHash.h \
			hash.h \
			NocaseStringHash.h \
			stack.h \
			queue.h \
			list.h
libcontain_la_LIBADD=libreplace/libreplace.la
AM_CFLAGS = -g -Wall -Werror -Iarch/include
AM_CXXFLAGS = -g -Wall -Werror -Iarch/include

# testsuite
#TESTS = test1 test2 test3 test6

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

