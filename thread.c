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
#define LT_NO_PTHREAD_WRAP
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "thread.h"
#include "semaphore.h"

static pthread_key_t key;
static void lt_thread_term(void);

static void * lt_thread_create_helper(void * arg)
{
	void * retval;
	lt_thread_t * handle = (lt_thread_t*)arg;

	pthread_setspecific(key, handle);
	retval = handle->func(handle->arg);
	pthread_setspecific(key, NULL);
	free(handle);

	return retval;
}

void lt_thread_init(void)
{
	atexit(lt_thread_term);
	pthread_key_create(&key, NULL);
}

static void lt_thread_term(void)
{
	pthread_key_delete(key);
}

lt_thread_t * lt_thread_new(
	lt_thread_func_t start_func, 
	void * arg
)
{
	lt_thread_t * retval = (lt_thread_t*)calloc(1, sizeof(lt_thread_t));
	int rc;
	
	retval->arg = arg;
	retval->func = start_func;

	retval->priv_sem = lt_sem_new(0);
	
	rc = pthread_create(
		&(retval->handle), 
		NULL, 
		lt_thread_create_helper, 
		retval
	);
	if (rc != 0)
	{
		lt_sem_free(retval->priv_sem);
		free(retval);
		retval = NULL;
	}
	
	return retval;
}

lt_thread_t * lt_thread_self(void)
{
	lt_thread_t * retval;

	retval = (lt_thread_t*)pthread_getspecific(key);

	return retval;
}

int lt_thread_eq(lt_thread_t * t1, lt_thread_t * t2)
{
	return pthread_equal(t1->handle, t2->handle) ? 0 : -1;
}

void lt_thread_kill(lt_thread_t * thread, int how)
{
	pthread_kill(thread->handle, how);
}

