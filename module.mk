arch_SRC = 	$(ARCH)/compare_and_exchange.c \
		$(ARCH)/set.c \
		$(ARCH)/increment.c \
		$(ARCH)/decrement.c

SRC += $(patsubst %,arch/%,$(arch_SRC))

