preprocessor_SRC += 	preproc.c \
			preprocl.l \
			preprocp.y

SRC += $(patsubst %,preprocessor/%,$(preprocessor_SRC))

