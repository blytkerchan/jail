libreplace_SRC += 	memdup.c \
			catstr.c \
			fexist.c \
			ltrim.c \
			rtrim.c \
			binary.c

SRC += $(patsubst %,libreplace/%,$(LTLIBOBJS:.lo=.c))
SRC += $(patsubst %,libreplace/%,$(libreplace_SRC))
libreplace_INSTALL_HEADERS += $(patsubst %.c,%.h,$(libreplace_SRC))

