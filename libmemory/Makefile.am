lib_LTLIBRARIES=libmemory.la
libmemory_la_SOURCES=	smr.c \
			hptr.c \
			arch/@ARCH@/compare_and_exchange.c \
			arch/@ARCH@/increment.c \
			arch/@ARCH@/decrement.c
pkginclude_HEADERS=hptr.h smr.h
TESTS=test1 test2
noinst_PROGRAMS=$(TESTS)
test1_LDADD=libmemory.la
test2_LDADD=libmemory.la
AM_CFLAGS=-g
man_MANS=libmemory.3
EXTRA_DIST=test1.c test2.c libmemory.pdf libmemory.sgml $(man_MANS) libmemory.3.sgml

doc : libmemory.pdf

libmemory.pdf : libmemory.sgml
	docbook2pdf $<

%.3 : %.3.sgml
	docbook2man $<

