# Makefile.am - process with automake and configure script..
SUBDIRS=libreplace
noinst_PROGRAMS = test1 test2 test3 test6
test1_SOURCES = test1.cc
test1_LDADD = libcontain.la
test2_SOURCES = test2.cc
test2_LDADD = libcontain.la
test3_SOURCES = test3.cc
test3_LDADD = libcontain.la
test6_SOURCES=test6.c
test6_LDADD=libcontain.la
lib_LTLIBRARIES = libcontain.la
libcontain_la_SOURCES = Hash.cc StringHash.cc IntHash.cc c_interface.cc NocaseStringHash.cc arch/@ARCH@/compare_and_exchange.c arch/@ARCH@/decrement.c arch/@ARCH@/increment.c
libcontain_la_LIBADD=libreplace/libreplace.la

pkginclude_HEADERS = Hash.h StringHash.h IntHash.h hash.h NocaseStringHash.h
AM_CFLAGS = -g -Wall -Werror -Iarch/include
AM_CXXFLAGS = -g -Wall -Werror -Iarch/include
TESTS = test1 test2 test3 test6
man7MANS = Hash.7
noinst_DATA = README
EXTRA_DIST=Hash.7 README.in Hash.7.pod arch
bin_SCRIPTS=config-libcontain
%.7 : %.7.pod
	pod2man --section 7 --release "The Jail project" --center "libcontain documentation" $< > $@

%.3 : %.3.pod
	pod2man --section 3 --release "The Jail project" --center "libcontain documentation" $< > $@

README : Hash.7.pod README.in
	pod2text $< > $@
	cat $(srcdir)/README.in >> $@

test6$(EXEEXT) : test6.$(OBJEXT)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) $(libcontain_la_LDFLAGS) -o $@ $^
