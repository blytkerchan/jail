SUBDIRS=posix

lib_LTLIBRARIES=libconf.la
libconf_la_SOURCES=libconf.c
pkginclude_HEADERS=libconf.h

man3_MAN=libconf.3
EXTRA_DIST=libconf.3 libconf.sgml

%3 : %.sgml
	docbook2man $<

