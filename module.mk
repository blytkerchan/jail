libconf_SRC += 	libconf.c \
		libconf_opt.c \
		libconf_optparam.c \
		posix/LCFlexLexer.cc \
		posix/readconf.cc

SRC += $(patsubst %,libconf/%,$(libconf_SRC))
