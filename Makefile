all : libthread.a

libthread.a : thread.o critical_section.o barrier.o

%.o : %.c barrier.h thread.h critical_section.h binary.h Makefile
	gcc -Wall -Werror -c -O2 -DNDEBUG -save-temps -o $@ $<

	
