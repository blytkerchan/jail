lib_LTLIBRARIES=libmemory.la
libmemory_la_SOURCES=smr.c hptr.c arch/@ARCH@/compare_and_exchange.c
pkginclude_HEADERS=hptr.h smr.h
EXTRA_DIST=test1.c test2.c libmemory.pdf libmemory.sgml

doc : libmemory.pdf

libmemory.pdf : libmemory.sgml
	docbook2pdf $<

