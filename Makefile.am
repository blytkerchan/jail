SUBDIRS=libcontain
bin_PROGRAMS=jailpp
jailpp_SOURCES=preprocl.l preprocp.y preproc.c
jailpp_LDADD=libcontain/libcontain.la
noinst_HEADERS=preproc.h
AM_YFLAGS=-d
AM_CFLAGS=-g

preprocp.h : preprocp.c
preprocl.c : preprocp.h
