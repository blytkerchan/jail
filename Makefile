CFLAGS=-g -Iarch/include -Wall -Werror
LDFLAGS=-Wall -Werror -g -O2
COMMON_OBJS=smr.o hptr.o arch/i486/increment.o arch/i486/decrement.o arch/i486/compare_and_exchange.o

all : test1 test2 

test1 : test1.o $(COMMON_OBJS)
	gcc $(LDFLAGS) -o $@ $^ -lpthread

test2 : test2.o $(COMMON_OBJS)
	gcc $(LDFLAGS) -o $@ $^ -lpthread

.c.o : Makefile smr.h hptr.h
	gcc -c $(CFLAGS) -o $@ $<

.PHONY: clean doc

clean : 
	rm -f *.o *.core test

doc : libmemory.pdf index.html

libmemory.pdf : libmemory.sgml
	docbook2pdf $<
	
index.html : libmemory.sgml
	docbook2html $<
