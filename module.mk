# the library
libcontain_CXX_SRC += 	Hash.cc \
			StringHash.cc \
			IntHash.cc \
			NocaseStringHash.cc \
			IniHash.cc \
			c_interface.cc
libcontain_SRC +=	stack.c \
			queue.c \
			hash.c \
			array.c \
			map.c \
			binomial_tree.c \
			binary.c \
			heap.c \
			prime.c \
			list.c
libcontain_GLIB_SRC = 	glib/ghash.c \
			glib/gprimes.c

SRC += $(patsubst %,libcontain/%,$(libcontain_SRC))
CXX_SRC += $(patsubst %,libcontain/%,$(libcontain_SRC))
GLIB_SRC += $(patsubst %,libcontain/%,$(libcontain_SRC))

