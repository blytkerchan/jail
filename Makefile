all : test

test : test.o
	$(CXX) -g -O2 -Wall -Werror -o $@ $^

%.o : %.cc dag.hpp $(wildcard detail/*.hpp)
	$(CXX) -g -O2 -c -Wall -Werror -o $@ $<

