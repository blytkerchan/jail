noinst_LTLIBRARIES=libconf_posix.la
libconf_posix_la_SOURCES=confp.y LCFlexLexer.cc readconf.cc
libconf_posix_la_LIBADD=confl.lo
noinst_HEADERS=LCFlexLexer.h readconf.h
EXTRA_DIST=confp.c confl.cc confp.h confl.l
AM_LFLAGS=-+
AM_YFLAGS=-d

confl.lo : confl.cc
confl.cc : confp.h
confp.h : confp.c

confl.cc : confl.l
	$(LEX) -+ -o$@ $<

