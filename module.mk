# the library
libcontain_SRC +=	stack.c \
			queue.c \
			hash.c \
			array.c \
			map.c \
			binomial_tree.c \
			heap.c \
			prime.c \
			list.c \
			vector.c \
			hash_helpers.c

SRC += $(patsubst %,libcontain/%,$(libcontain_SRC))

