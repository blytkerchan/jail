SUBDIRS=libcontain
bin_PROGRAMS=jailpp
jailpp_SOURCES=preprocl.l preprocp.y
jailpp_LDADD=libcontain/libcontain.la
AM_YFLAGS=-d
AM_CFLAGS=-g

preprocp.h : preprocp.c
preprocl.c : preprocp.h
