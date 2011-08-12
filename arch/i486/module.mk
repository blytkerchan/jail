libarch_SRC = 	compare_and_exchange.c \
		set.c \
		increment.c \
		decrement.c \
		fetch_and_add.c

SRC += $(patsubst %,libarch/%,$(libarch_SRC))

