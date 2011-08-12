noinst_LTLIBRARIES=libconf_posix.la
libconf_posix_la_SOURCES=confp.y readconf.cc
libconf_posix_la_LIBADD=confl.lo -lfl
noinst_HEADERS=readconf.h
EXTRA_DIST=confp.c confp.h confl.l
AM_CFLAGS=-I$(top_srcdir)
AM_CXXFLAGS=-I$(top_srcdir)
if MT_SAFE
EXTRA_DIST+=confl.cc
noinst_HEADERS+=LCFlexLexer.h
libconf_posix_la_SOURCES+=LCFlexLexer.cc
confl.lo : confl.cc
confl.cc : confl.l confp.h
	$(LEX) -+ -o$@ $<
AM_CFLAGS+=-DMT_SAFE
AM_CXXFLAGS+=-DMT_SAFE
else
EXTRA_DIST+=confl.c
confl.lo : confl.c
confl.c : confl.l confp.h
	$(LEX) -o$@ $<
endif
AM_YFLAGS=-d

confp.h : confp.c


