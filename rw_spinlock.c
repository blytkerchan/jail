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
#define EXCL 1
#define SHRD 0x10000

#include "rw_spinlock.h"
#include "arch/include/fetch_and_add.h"

/* This file contains an abstracted implementation of the starvation-free 
 * read/write spinlock algorith as posted by Joe Seigh in 
 * http://tinyurl.com/4ar25, reproduced with his permission 
 * (http://tinyurl.com/5whfb) */

/* Joe Seigh's explanation: (from http://tinyurl.com/4ar25)
 * They're bakery algorithms with FIFO service order. In the first exclusive
 * requests wait for *all* prior accesses to complete. Shared requests just
 * wait for prior exclusive accesses to complete.  Number of waiting threads 
 * has to be less than the field sizes.  The algorithm works even with wrap 
 * and carry out from the exclusive access field into the shared access field.
 *
 * [snip another version with fetch-and-increment]
 * 
 * You can optimize this somewhat by using a power of 2 to define the field
 * size and logically anding to extract a field value, using a store w/ membar 
 * to release exclusive access on the first one.
 *
 * Bakery spinlocks may be more efficient than normal spinlocks since they 
 * acquire their wait ticket value first and wait rather than waiting and 
 * then all waiting threads attempting to acquire the lock all in the same 
 * instant.  The interlocked updates from ticket requests are more spread 
 * out and cause less contention on the cache.*/

rw_spinlock_t * rw_spinlock_new(void)
{
	rw_spinlock_t * retval;

	retval = calloc(1, sizeof(rw_spinlock_t));

	return retval;
}

void rw_spinlock_free(rw_spinlock_t * handle)
{
	free(handle);
}

void rw_spinlock_read_lock(rw_spinlock_t * handle)
{
	int wait; /* wait ticket */

	wait = fetch_and_add(&(handle->next), SHRD);
	while ((wait % SHRD) != (handle->curr % SHRD));
}

void rw_spinlock_read_unlock(rw_spinlock_t * handle)
{
	fetch_and_add(&(handle->curr), SHRD);
}

void rw_spinlock_write_lock(rw_spinlock_t * handle)
{
	int wait; /* wait ticket */

	wait = fetch_and_add(&(handle->next), EXCL);
	while (wait != handle->curr);
}

void rw_spinlock_write_unlock(rw_spinlock_t * handle)
{
	fetch_and_add(&(handle->curr), EXCL);
}

