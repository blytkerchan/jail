SOURCES=depends.c vector.c node.c edge.c
OBJS=$(patsubst %.c,%.o,$(SOURCES))

all : $(OBJS)


%.o : %.c
	gcc -c -Wall -Werror -I$(HOME)/opt/include -o $@ $<
