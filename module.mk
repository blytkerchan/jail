preprocessor_SRC += 	preproc.c \
			preprocl.l \
			preprocp.y

SRC += $(patsubst %,preprocessor/%,$(preprocessor_SRC))

preprocl.c : preprocl.l
	flex --yylineno -o $@ 
preprocp.c : preprocp.y

