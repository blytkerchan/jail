COMPILE_CMD=libtool --mode=compile
LINK_CMD=libtool --mode=link
CFLAGS=-g -I ~/opt/include -Wall -Werror
HEADERS=libconf.h

all : libconf.la

libconf.la : libconf.lo
	$(LINK_CMD) $(CC) -o $@ $^
	
%.lo : %.c Makefile $(HEADERS)
	$(COMPILE_CMD) $(CC) $(CFLAGS) -c -o $@ $<
