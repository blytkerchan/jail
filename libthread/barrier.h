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
#ifndef _LIBTHREAD_BARRIER_H_
#define _LIBTHREAD_BARRIER_H_

#include <stdint.h>

typedef struct 
{
	uint32_t stages;
	uint32_t * arrive;
	uint32_t index;
	uint32_t num_threads;
} barrier_t;

/* Create a new barrier.
 * This function creates a new barrier that will allow NUM_THREADS processes
 * to synchronize. The created barrier is initialized in such a way that it
 * is ready for use by barrier_wait(). Do not use this barrier to synchronize
 * a number of threads that is not exactly NUM_THREADS! */
barrier_t * barrier_new(uint32_t num_threads);

/* Free the resources associated with a barrier.
 * This function frees all resources associated with a barrier, rendering it
 * unusable. Use this when you no longer need the barrier. */
void barrier_free(barrier_t * barrier);

/* Synchronize the current thread with other threads on this barrier.
 * This function will wait until NUM_THREADS threads have arrived at the 
 * barrier, including the calling thread. "Arriving" at the barrier means
 * calling this function. This function will only return if it is concurrently
 * called by NUM_THREADS threads. */
void barrier_wait(barrier_t * barrier);

#endif // _LIBTHREAD_BARRIER_H_

