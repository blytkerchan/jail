/* Jail: Just Another Interpreted Language
 * Copyright (c) 2004, Ronald Landheer-Cieslak
 * All rights reserved
 * 
 * This is free software. You may distribute it and/or modify it and
 * distribute modified forms provided that the following terms are met:
 *
 * * Redistributions of the source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer;
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the distribution;
 * * None of the names of the authors of this software may be used to endorse
 *   or promote this software, derived software or any distribution of this 
 *   software or any distribution of which this software is part, without 
 *   prior written permission from the authors involved;
 * * Unless you have received a written statement from Ronald Landheer-Cieslak
 *   that says otherwise, the terms of the GNU General Public License, as 
 *   published by the Free Software Foundation, version 2 or (at your option)
 *   any later version, also apply.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <pthread.h>
#include <signal.h>
#include "semaphore.h"
#include "thread.h"
#include "arch/include/set.h"
#include "arch/include/compare_and_exchange.h"
#include "arch/include/increment.h"
#include "arch/include/decrement.h"
#include "libmemory/smr.h"
#include "libmemory/hptr.h"

static lt_thread_t * lt_sem_first(lt_sem_t * sem);
static void lt_sem_enq(lt_sem_t * sem, lt_thread_t * thread);
static void lt_sem_deq(lt_sem_t * sem);

/* Initialize the given semaphore to the given value
 * Note: we don't even try to fail if the semaphore is in use: the behaviour
 * is undefined, so basically we don't give a ... (fill in your favorite
 * obscenity) */
void lt_sem_init(lt_sem_t * semaphore, uint32_t val)
{
	atomic_set(&(semaphore->value), (void*)val);
}

/* Create a new semaphore with the given value */
lt_sem_t * lt_sem_new(uint32_t val)
{
	lt_sem_t * retval;

	retval = (lt_sem_t*)malloc(sizeof(lt_sem_t));
	retval->value = val;
	retval->queue = NULL;
	retval->tail = NULL;
	
	return retval;
}

/* Free a semaphore and the related resources */
void lt_sem_free(lt_sem_t * semaphore)
{
	free(semaphore);
}

/* Wait on a semaphore */
void lt_sem_wait(lt_sem_t * sem)
{
	uint32_t value;		// local copy of the sem value
	sigset_t mask, o_mask;	// signal masks we work on (new & old)
	
	// set the signal mask for the calling thread. This prevents SIGCONT
	// signals from being ignored in the waiting loop
	sigemptyset(&mask);
	sigaddset(&mask, SIGCONT);
	pthread_sigmask(SIG_BLOCK, &mask, &o_mask);

	// first, put us on the queue. This ensures we don't miss any
	// wake-up calls and nobody will miss us. It also ensures FIFO on the
	// semaphore, making it fair.
	lt_sem_enq(sem, lt_thread_self());
	// this is the waiting loop. When we get out of this,
	// the semaphore is ours
	while (1) 
	{
		// now, see if we're the first on the list
		if (lt_thread_eq(lt_sem_first(sem), lt_thread_self()) == 0)
		{	// once we're the first in the list, that will always be the case 
			// until we remove ourselves. No other thread has the right to do
			// that
			// get the current value of the semaphore
			value = sem->value;
			if (value > 0) {
				// try to get the semaphore
				if (compare_and_exchange_int(&value, &(sem->value), value - 1) != 0)
				{
					continue;	// try again
				} else break;	// OK
			}
		} 
		else 
		{	// semaphore is locked and/or we're not the first waiting
			// wait for it to be unlocked
			sigsuspend(&mask);
		}
	}
	
	// set the old mask back
	pthread_sigmask(SIG_SETMASK, &o_mask, NULL);
}

void lt_sem_release(lt_sem_t * sem)
{
	lt_thread_t * first;

	// check whether we were ever queued - if so, we're the first in the queue
	if (lt_thread_eq(lt_sem_first(sem), lt_thread_self()) == 0)
		lt_sem_deq(sem);
	// release the semaphore
	atomic_increment(&(sem->value));
	// signal the first waiting thread
	do
	{
		if ((first = lt_sem_first(sem)) != NULL)
			lt_thread_kill(first, SIGCONT);
	} while (first == lt_sem_first(sem));
	// done
}

static void lt_sem_enq(lt_sem_t * sem, lt_thread_t * thread)
{
	lt_thread_t * old_tail;
	lt_thread_t * old_next;
	
	while (1)
	{
		do
		{
			old_tail = sem->tail;
			hptr_register(0, old_tail);
		} while (old_tail != sem->tail);
		old_next = old_tail->next;
		if (old_tail != sem->tail)
			continue;
		if (old_next != NULL)
		{
			compare_and_exchange_ptr(&old_tail, &(sem->tail), old_next);
			continue;
		}
		if (compare_and_exchange_ptr(&old_next, &(old_tail->next), thread) == 0)
			break;
	}
	compare_and_exchange_ptr(&old_tail, &(sem->tail), thread);
	hptr_free(0);
}

/* NOTE: this function is *not* thread-safe: we assume that only one
 * thread will call this function at any time! */
static void lt_sem_deq(lt_sem_t * sem)
{
	lt_thread_t * first;
	lt_thread_t * next;
	
	first = sem->queue;
	next = first->next;
	atomic_set(&(sem->queue), next);
	if (sem->queue == NULL)
		atomic_set(&(sem->tail), NULL);
}

static lt_thread_t * lt_sem_first(lt_sem_t * sem)
{
	lt_thread_t * first;
	
	do
	{
		first = sem->queue;
		hptr_register(0, first);
	} while (first != sem->queue);

	return first;
}

