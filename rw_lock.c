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
#include <assert.h>
#include "arch/include/compare_and_exchange.h"
#include "rw_lock.h"

#define READER 1
#define WRITER 2

static void lt_rwlock_schedule(lt_rwlock_t * lock);
static int lt_rwlock_list_remove(lt_thread_t * list, lt_thread_t * entry);
static int lt_rwlock_queue_empty(lt_thread_t * queue);
static int lt_rwlock_list_empty(lt_thread_t * list);
static void lt_thread_wake(lt_thread_t * thread);
static lt_thread_t * lt_rwlock_queue_first(lt_thread_t * queue);
static int atomic_swap_ptr(volatile void * ptr1, volatile void * ptr2);
static lt_thread_t * lt_rwlock_queue_deq(lt_thread_t * queue);
static void lt_rwlock_list_move(lt_thread_t * to, lt_thread_t * from);
static void lt_rwlock_list_for_each(lt_thread_t * list, void (*)(lt_thread_t * thread));
static void lt_rwlock_queue_enq(lt_thread_t * queue, lt_thread_t * thread);
static void lt_thread_suspend(lt_thread_t * thread);
static void lt_rwlock_list_insert(lt_thread_t * list, lt_thread_t * thread);
static lt_thread_t * lt_rwlock_list_find(lt_thread_t * list, lt_rwlock_t * thread);

void lt_rwlock_read_lock(lt_rwlock_t * lock)
{
	/* first, we set a flag in our thread descriptor that identifies us
	 * as a reader */
	lt_thread_self()->flag = READER;

	/* now, we have ourselves scheduled
	 * when we get out of this one, we are not only scheduled, but we can
	 * continue through the lock as a reader */
	lt_rwlock_schedule(lock);
}

void lt_rwlock_write_lock(lt_rwlock_t * lock)
{
	/* first, we set a flag in our thread descriptor that identifies us
	 * as a writer */
	lt_thread_self()->flag = WRITER;

	/* now, we have ourselves scheduled
	 * when we get out of this one, we are not only scheduled, but we can
	 * continue through the lock as a writer */
	lt_rwlock_schedule(lock);
}

void lt_rwlock_read_unlock(lt_rwlock_t * lock) 
{
	/* first, we get ourselves out of the readers list */
	assert(lt_rwlock_list_remove(lock->readers[1], lt_thread_self()) == 0);
	/* now, if the readers list is empty, there are no more active readers
	 * and a writer is therefore eligable for the lock. Otherwise, we're
	 * done */
	if (lt_rwlock_queue_empty(lock->writers) != 0) 
	{
		if (lt_rwlock_list_empty(lock->readers[1]) == 0) 
		{
			lt_thread_wake(lt_rwlock_queue_first(lock->writers));
		}
	}
}

void lt_rwlock_write_unlock(lt_rwlock_t * lock) 
{
	lt_thread_t * place_holder = NULL;

	/* As of this point, arriving readers should go into readers[1], the
	 * readers in readers[0] should go there too, and readers[1] should 
	 * be empty.
	 * Hence, we us place-holder lists for arriving readers, which we will
	 * add to readers[1] once we're sure the scheduler will take the proper
	 * list place_holder: empty list of threads;
	 *
	 * we assume readers[1] is empty */
	assert(lt_rwlock_list_empty(lock->readers[1]));
	/* so we put the place-holder at readers[0] */
	atomic_swap_ptr(lock->readers[0], place_holder);
	/* place_holder now contains readers[0]
	 * we now de-queue ourselves */
	assert(lt_thread_eq(lt_rwlock_queue_deq(lock->writers), lt_thread_self()) == 0);
	/* if the writers queue is empty, it will schedule into readers[1];
	 * if not, it will continue scheduling into readers[0], which is now the
	 * place-holder. */
	/* we add whatever we swapped out to readers[1] */
	lt_rwlock_list_move(lock->readers[1], place_holder);
	/* we swap again. As readers[0] only contains sleeping threads, we're sure
	 * all the threads in there are asleep and won't think they're in
	 * readers[1] */
	atomic_swap_ptr(place_holder, lock->readers[0]);
	/* we move whatever we got from readers[0] between the time put the 
	 * place-holder in place and the time we removed ourselves from the writers
	 * queue (which may have made it possible to schedule into readers[1]
	 * to readers[1]. If the scheduler is still scheduling into readers[0],
	 * these threads just got lucky */
	lt_rwlock_list_move(lock->readers[1], place_holder);
	/* now, we awake everyone in readers[1] */
	lt_rwlock_list_for_each(lock->readers[1], lt_thread_wake);
	/* and we're done */
}

void lt_rwlock_schedule(lt_rwlock_t * lock) 
{
	lt_thread_t * curr;

	/* now, the local thread is enqueued in the general queue. This guarantees
	 * that at some time in the future, it will be scheduled */
	lt_rwlock_queue_enq(lock->general, lt_thread_self());

	/* now, if it's not the first one in the queue, it should do its own
	 * scheduling. Otherwise, some other thread will take care of it and
	 * we can suspend ourselves. */
	if (lt_thread_eq(lt_rwlock_queue_first(lock->general), lt_thread_self()) != 0) 
	{
		lt_thread_suspend(lt_thread_self());
	}
	else 
	{
		/* we were the first, so either someone is already scheduling is, or 
		 * we start scheduling everybody behind us until the general queue is
		 * empty. */
		void * exp;

		/* try to become the scheduler */
		exp = NULL;
		if (compare_and_exchange_ptr(&exp, &(lock->scheduler), lt_thread_self()) != 0) {
			/* some other thread is already scheduling - let him do the work */
			lt_thread_suspend(lt_thread_self());
		} else {
			/* we're the scheduler. Because of this, we know we're the only thread
			 * adding to the two readers lists and the writers queue, and removing
			 * from the general queue. */

			while (!lt_rwlock_queue_empty(lock->general)) 
			{
				/* get the next thread to schedule */
				curr = lt_rwlock_queue_deq(lock->general);
				/* note that we don't handle ourselves specially here: when we're
				 * done, we'll see where we are and act accordingly */
				switch (curr->flag)
				{
				case READER :
					if (lt_rwlock_queue_empty(lock->writers)) 
					{
						/* if there are no writers, the thread (which is a reader) 
						 * goes to the second readers list and can go on 
						 * immediatly. */
						lt_rwlock_list_insert(lock->readers[1], curr);
						lt_thread_wake(curr);
					} 
					else 
					{
						/* if there are writers, the thread will have to continue 
						 * sleeping until the writers are done. */
						lt_rwlock_list_insert(lock->readers[0], curr);
					}
					break;
				case WRITER :
					/* in any case, we enqueue the writer */
					lt_rwlock_queue_enq(lock->writers, curr);
					/* we then look whether there are any active readers (in 
					 * lock->readers[1]). If not, we wake the writer */
					if (lt_rwlock_list_empty(lock->readers[1])) 
						lt_thread_wake(curr);
					break;
				}
			}
			/* now, there are no more threads to schedule, so we're done. */
			exp = lt_thread_self();
			assert(compare_and_exchange_ptr(&exp, &(lock->scheduler), NULL) == 0);
			/* if there are any threads in the general queue now, we wake up the
			 * first and let him handle the scheduling */
			if (!lt_rwlock_queue_empty(lock->general))
				lt_thread_wake(lt_rwlock_queue_first(lock->general));
		}
	}	

	while (1) 
	{
		/* when we get here, we've either been scheduling the other threads or
		 * we've been woken up. We check which it is by checking where we are. */
		switch (lt_thread_self()->flag)
		{
		case READER :
			/* readers can continue if they're in the second list */
			if (lt_rwlock_list_find(lock->readers[1], lt_thread_self()) == NULL)
				return; /* break out of the endless loop */
			break;
		case WRITER :
			/* writers can continue if they're the first one in the queue
			 * and the second readers list is empty */
			if (lt_rwlock_list_empty(lock->readers[1]) && 
				(lt_thread_eq(lt_thread_self(), lt_rwlock_queue_first(lock->writers)) == 0))
				return; /* break out of the endless loop */
			break;
		}
		/* we didn't break out of the loop, so we sleep */
		lt_thread_suspend(lt_thread_self());
	}
}

