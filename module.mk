SRC += 	libreplace/memdup.c \
	libreplace/catstr.c \
	libreplace/fexist.c \
	libreplace/ltrim.c \
	libreplace/rtrim.c
SRC += $(patsubst %,libreplace/%,$(LTLIBOBJS:.lo=.c))

