# the library
@CXX_TRUE@SRC += 	libcontain/Hash.cc \
@CXX_TRUE@	libcontain/StringHash.cc \
@CXX_TRUE@	libcontain/IntHash.cc \
@CXX_TRUE@	libcontain/NocaseStringHash.cc \
@CXX_TRUE@	libcontain/IniHash.cc \
@CXX_TRUE@	libcontain/c_interface.cc
SRC +=	libcontain/stack.c \
	libcontain/queue.c \
	libcontain/hash.c \
	libcontain/array.c \
	libcontain/map.c \
	libcontain/binomial_tree.c \
	libcontain/binary.c \
	libcontain/heap.c \
	libcontain/prime.c \
	libcontain/list.c
