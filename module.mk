# the library
libcontain_SRC +=	stack.c \
			queue.c \
			hash.c \
			array.c \
			map.c \
			binomial_tree.c \
			heap.c \
			prime.c \
			list.c
libcontain_GLIB_SRC = 	glib/ghash.c \
			glib/gprimes.c

SRC += $(patsubst %,libcontain/%,$(libcontain_SRC))
GLIB_SRC += $(patsubst %,libcontain/%,$(libcontain_SRC))

