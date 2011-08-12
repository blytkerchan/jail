all : parse.o

parse.o : parse.c
	gcc -c -Wall -Werror -o $@ $<

parse.c : parse.y
	bison -d -g -o $@ -v $<

