#include "semaphore.h"

int lt_sem_init(lt_sem_t * semaphore)
{
	return -1;
}

lt_sem_t * lt_sem_create(uint32_t val)
{
	return NULL;
}

void lt_sem_free(lt_sem_t * semaphore)
{
}

void lt_sem_wait(lt_sem_t * semaphore)
{
/* waits on a semaphore. This follows the following algorithm:
with:
lt_sem_t : struct {
	value : int >= 0;				# the curr value of the semaphore
	queue : queue of thread_t;	# queue of waiting threads
}
lt_sem_wait(sem : lt_sem_t) {
	value : int >= 0;				# local copy of the curr value of the semaphore
	mask, o_mask : sigset_t;	# signal mask (new & old)
	action, o_action : sigaction;

	# set the signal mask for the calling thread. This prevents SIGCONT signals
	# from being ignored in the waiting loop
	sigemptyset(mask);
	sigaddset(mask, SIGCONT);
	pthtread_sigmask(SIG_BLOCK, mask, o_mask);

	# this is the waiting loop. When we get out of this, the semaphore is ours
	while (1) {
		# get the current value of the semaphore
		value = sem^.value;
		if (value > 0) {
			# try to get the semaphore
			if (CAS(value, sem^.value, value - 1) != OK) {
				continue;	# try again
			} else break;	# OK
		} else {	# semaphore is locked
			# wait for it to be unlocked
			enq(sem^.queue, thread_self);
			sigsuspend(mask);
		}
	}
	# set the old mask back
	pthread_sigmask(SIG_SETMASK, o_mask, NULL);
}
Though this favours FIFO wrt the fact that the current thread is queued (and
won't be signalled by lt_sem_release described below unless it is the first in 
the queue), but does not guarantee it because it may be that an arriving thread
takes the semaphore before a newly awoken one.. This has the potential of 
starving waiting threads(!). An alternative would be to pass the baton without
releasing the semaphore, but that would mean that the awoken thread would have
some other way of knowing it was awoken for the semaphore, and nothing else.
*/
}

void lt_sem_release(lt_sem_t * semaphore)
{
/*releases a semaphore when it's done with it. It awakes a 
waiting thread, but does not guarantee that that thread will actually get the
semaphore. The algorithm is described below, with lt_sem_t the same as above:
lt_sem_release(sem : lt_sem_t) {
	value : int >= 0;

	# check whether we were ever queued - if so, we're the first in the queue
	if (first(sem^.queue) == thread_self()) {
		deq(sem^.queue);
	}
	# release the semaphore
	atomic_increment(sem^.value);
	# signal the first waiting thread
	kill(SIGCONT, first(sem^.queue));
	# done
}

Note: SUSv3 doesn't specify what happens if we sigsuspend() waiting for a signal
that is ignored. It would be silly to ignore SIGCONT, but it can be done. Doing
so will cause unspecified behaviour in this implementation.
*/
}


