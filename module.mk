libthread_SRC := 	barrier.c \
			critical_section.c \
			thread.c \
			rw_spinlock.c \
			rw_lock.c \
			thread_list.c \
			thread_queue.c \
			rwlock.c

ifeq ($(HAVE_PTHREAD_SIGMASK),1)
libthread_SRC +=	semaphore.c
endif
SRC += $(patsubst %,libthread/%,$(libthread_SRC))
libthread_INSTALL_HEADERS += $(patsubst %.c,%.h,$(libthread_SRC))
INSTALL_HEADERS += $(patsubst %,libthread/%,$(libthread_INSTALL_HEADERS))

