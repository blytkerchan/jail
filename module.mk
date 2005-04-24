libmemory_SRC +=	smr.c \
			hptr.c

SRC += $(patsubst %,libmemory/%,$(libmemory_SRC))
libmemory_INSTALL_HEADERS += $(patsubst %.c,%.h,$(libmemory_SRC))
INSTALL_HEADERS += $(patsubst %,libmemory/%,$(libmemory_INSTALL_HEADERS))

