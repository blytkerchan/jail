libthread_SRC := 	barrier.c \
			critical_section.c \
			semaphore.c \
			thread.c
SRC += $(patsubst %,libthread/%,$(libthread_SRC))

