all : test

test : main.o compare_and_exchange.o
	gcc -o $@ $^

.c.o :
	gcc -save-temps -g -c -o $@ $<

clean :
	rm -f test *.s *.i *.o
