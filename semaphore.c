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

static thread_t * lt_sem_first(lt_sem_t * sem);
static void lt_sem_enq(lt_sem_t * sem, thread_t * thread);
static void lt_sem_deq(lt_sem_t * sem);

/* Initialize the given semaphore to the given value
 * Note: we don't even try to fail if the semaphore is in use: the behaviour
 * is undefined, so basically we don't give a ... (fill in your favorite
 * obscenity) */
void lt_sem_init(lt_sem_t * semaphore, uint32_t val)
{
	atomic_set(&(semaphore->value), val);
}

/* Create a new semaphore with the given value */
lt_sem_t * lt_sem_create(uint32_t val)
{
	lt_sem_t * retval;

	retval = (lt_sem_t*)malloc(sizeof(lt_sem_t));
	retval->value = val;
	retval->queue = NULL;
	
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
	
	// set the signal mask for the calling thread. This prevents SIGCONT\
	// signals from being ignored in the waiting loop
	sigemptyset(mask);
	sigaddset(mask, SIGCONT);
	pthtread_sigmask(SIG_BLOCK, mask, o_mask);

	// this is the waiting loop. When we get out of this,
	// the semaphore is ours
	while (1) 
	{
		// get the current value of the semaphore
		value = sem->value;
		if (value > 0) {
			// try to get the semaphore
			if (compare_and_exchange(&value, &(sem->value), value - 1) != 0)
			{
				continue;	// try again
			} else break;	// OK
		} else {	// semaphore is locked
			// wait for it to be unlocked
			lt_sem_enq(sem, thread_self());
			sigsuspend(mask);
		}
	}
	
	// set the old mask back
	pthread_sigmask(SIG_SETMASK, o_mask, NULL);
}

void lt_sem_release(lt_sem_t * sem)
{
	uint32_t value;
	thread_t * first;

	// check whether we were ever queued - if so, we're the first in the queue
	if (thread_eq(lt_sem_first(sem), thread_self()) == 0)
		lt_sem_deq(sem);
	// release the semaphore
	atomic_increment(&(sem->value));
	// signal the first waiting thread
	do
	{
		if ((first = lt_sem_first(sem)) != NULL)
			thread_kill(SIGCONT, first);
	} while (first == lt_sem_first(sem));
	// done
}

static void lt_sem_enq(lt_sem_t * sem, thread_t * thread)
{
}

static lt_thread_t * lt_sem_deq(lt_sem_t * sem)
{
	thread_t * first;
	thread_t * next;
	
	while (1)
	{
		do
		{
			first = sem->first;
			hptr_register(0, first);
		} while (first != sem->first);
		next = first->next;
		if (compare_and_exchange(&first, &(sem->first), next) == 0)
			break;
	}

	return first;
}

static thread_t * lt_sem_first(lt_sem_t * sem)
{
	thread_t * first;
	
	do
	{
		first = sem->first;
		hptr_register(0, first);
	} while (first != sem->first);

	return first;
}

