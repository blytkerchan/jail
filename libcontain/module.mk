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
			list_node.c \
			vector.c \
			hash_helpers.c
libcontain_INSTALL_HEADERS += $(patsubst %.c,%.h,$(libcontain_SRC)) libcontain_config.h types.h
SRC += $(patsubst %,libcontain/%,$(libcontain_SRC))
INSTALL_HEADERS += $(patsubst %,libcontain/%,$(libcontain_INSTALL_HEADERS))

