all : confp.c LCFlexLexer.o confl.o

%.c : %.y
	bison -d -o $@ $<

%.o : %.cc
	g++ -Wall -I.. -c -o $@ $< 

%.cc : %.l
	flex -o$@ -+ $<

.PRECIOUS : confl.cc

