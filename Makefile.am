SUBDIRS=libmemory libcontain posix libreplace . tests

lib_LTLIBRARIES=libconf.la
libconf_la_SOURCES=libconf.c libconf_opt.c libconf_optparam.c
pkginclude_HEADERS=libconf.h
libconf_la_LIBADD=libreplace/libreplace.la posix/libconf_posix.la libcontain/libcontain.la

man3_MAN=libconf.3
EXTRA_DIST=libconf.3 libconf.sgml

%3 : %.sgml
	docbook2man $<

