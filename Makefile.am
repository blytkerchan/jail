# Makefile.am - process with automake and configure script..
SUBDIRS=glib# tests 
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
			  NocaseStringHash.h \
			  singleton.h
else
libcontain_la_CXXSOURCES=
libcontain_la_CXXHEADERS=
endif
libcontain_la_SOURCES=	stack.c \
			queue.c \
			hash.c \
			array.c \
			map.c \
			binomial_tree.c \
			binary.c \
			heap.c \
			prime.c \
			list.c $(libcontain_la_CXXSOURCES)
pkginclude_HEADERS =	hash_config.h \
			hash.h \
			stack.h \
			queue.h \
			array.h \
			map.h \
			binomial_tree.h \
			heap.h \
			list.h $(libcontain_la_CXXHEADERS)
noinst_HEADERS = c_interface.h binary.h prime.h
libcontain_la_LDFLAGS=-version-info 2:0:1
AM_CFLAGS = -g -Wall -Werror -I$(top_srcdir)
AM_CXXFLAGS = -g -Wall -Werror -I$(top_srcdir)

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


