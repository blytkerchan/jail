libmemory_SRC +=	smr.c \
			hptr.c

SRC += $(patsubst %,libmemory/%,$(libmemory_SRC))
