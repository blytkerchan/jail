# Makefile.am - process with automake and configure script..
# stuff we need
DIST_SUBDIRS=libreplace glib tests
if GLIB
SUBDIRS=libreplace glib . tests
else
SUBDIRS=libreplace . tests
endif
EXTRA_DIST=Hash.7 README.in Hash.7.pod arch hash_config.h.in

# the library
lib_LTLIBRARIES = libcontain.la
if CXX
libcontain_la_CXXSOURCES= Hash.cc \
                          StringHash.cc \
                          IntHash.cc \
                          NocaseStringHash.cc \
                          IniHash.cc \
			  c_interface.cc
libcontain_la_CXXHEADERS= Hash.h \
                          StringHash.h \
			  IntHash.h \
			  IniHash.h \
			  NocaseStringHash.h
else
libcontain_la_CXXSOURCES=
libcontain_la_CXXHEADERS=
endif
libcontain_la_SOURCES = arch/@ARCH@/compare_and_exchange.c \
			arch/@ARCH@/decrement.c \
			arch/@ARCH@/increment.c \
			arch/@ARCH@/set.c \
			stack.c \
			queue.c \
			hash.c \
			array.c \
			map.c \
			binomial_tree.c \
			list.c $(libcontain_la_CXXSOURCES)
pkginclude_HEADERS =	hash_config.h \
			hash.h \
			stack.h \
			queue.h \
			array.h \
			map.h \
			binomial_tree.h \
			list.h $(libcontain_la_CXXHEADERS)
noinst_HEADERS = c_interface.h
libcontain_la_LIBADD=libreplace/libreplace.la glib/libglib.la
libcontain_la_LDFLAGS=-version-info 2:0:1
AM_CFLAGS = -g -Wall -Werror -I$(top_srcdir)/arch/include
AM_CXXFLAGS = -g -Wall -Werror -I$(top_srcdir)/arch/include

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
	cat README.in > $@
	pod2text $< >> $@


