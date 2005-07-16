INSTALL-HEADERS=dag.hpp depends.hpp detail/dag_iterator.hpp detail/visitors.hpp detail/circular_reference_exception.hpp detail/dag_node.hpp detail/scoped_flag.hpp

all : test

test : test.o
	$(CXX) -g -O2 -Wall -Werror -o $@ $^

%.o : %.cc dag.hpp $(wildcard detail/*.hpp)
	$(CXX) -g -O2 -c -Wall -Werror -o $@ $<

install :
	for h in $(INSTALL-HEADERS); do mkdir -p $(DESTDIR)/include/rlc/$$(dirname $$h); cp $$h $(DESTDIR)/include/rlc/$$(dirname $$h); done
