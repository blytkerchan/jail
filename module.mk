# the library
CXX_SRC += 	libcontain/Hash.cc \
		libcontain/StringHash.cc \
		libcontain/IntHash.cc \
		libcontain/NocaseStringHash.cc \
		libcontain/IniHash.cc \
		libcontain/c_interface.cc
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
GLIB_SRC += 	glib/ghash.c \
		glib/gprimes.c
