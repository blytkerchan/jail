libreplace_SRC += 	memdup.c \
			catstr.c \
			fexist.c \
			ltrim.c \
			rtrim.c
SRC += $(patsubst %,libreplace/%,$(LTLIBOBJS:.lo=.c))
SRC += $(patsubst %,libreplace/%,$(libreplace_SRC))

