libthread_SRC := 	barrier.c \
			critical_section.c \
			thread.c \
			rw_spinlock.c

ifeq ($(HAVE_PTHREAD_SIGMASK),1)
libthread_SRC +=	semaphore.c
endif
SRC += $(patsubst %,libthread/%,$(libthread_SRC))

