# Makefile.am - process with automake and configure script..
noinst_PROGRAMS = test1 test2 test3 test6
test1_SOURCES = test1.cc
test1_LDADD = libhash.la
test2_SOURCES = test2.cc
test2_LDADD = libhash.la
test3_SOURCES = test3.cc
test3_LDADD = libhash.la
test6_SOURCES=test6.c
test6_LDADD=libhash.la
lib_LTLIBRARIES = libhash.la
libhash_la_SOURCES = Hash.cc StringHash.cc IntHash.cc c_interface.cc NocaseStringHash.cc
libhash_la_LDFLAGS=`config-swing --libs`
pkginclude_HEADERS = Hash.h StringHash.h IntHash.h hash.h
AM_CFLAGS = -g -Wall -Werror `config-swing --cflags`
AM_CXXFLAGS = -g -Wall -Werror `config-swing --cflags`
TESTS = test1 test2 test3 test6
man7MANS = Hash.7
noinst_DATA = README
EXTRA_DIST = Hash.7 README.in Hash.7.pod
bin_SCRIPTS=config-libhash
%.7 : %.7.pod
	pod2man --section 7 --release "Real Life Systems" --center "Real Life Systems Hash Documentation" $< > $@

%.3 : %.3.pod
	pod2man --section 3 --release "Real Life Systems" --center "Real Life Systems Hash Documentation" $< > $@

README : Hash.7.pod README.in
	pod2text $< > $@
	cat README.in >> $@

test6$(EXEEXT) : test6.$(OBJEXT)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) $(libhash_la_LDFLAGS) -o $@ $^
