arch_SRC = 	$(ARCH)/compare_and_exchange.c \
		$(ARCH)/set.c \
		$(ARCH)/increment.c \
		$(ARCH)/decrement.c \
		$(ARCH)/fetch_and_add.c

arch_CFLAGS=-I$(srcdir)/arch/include

SRC += $(patsubst %,arch/%,$(arch_SRC))

