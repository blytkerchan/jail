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
#include <stdlib.h>
#include "barrier.h"
#include "binary.h"
#include "thread.h"
#include "arch/include/increment.h"
#include "arch/include/set.h"
#include "arch/include/compare_and_exchange.h"

/* blindingly obvious... */
barrier_t * barrier_new(uint32_t num_threads)
{
	barrier_t * retval;

	retval = calloc(1, sizeof(barrier_t));
	assert(retval != NULL);
	/* note that the number of stages is initialized only once. See the
	 * comments on the barrier_wait function below for an explanation
	 * on the initialization value. This is ceil(lg(num_threads)), BTW. */
	retval->stages = lg(num_threads - 1) + 1;
	retval->arrive = calloc(num_threads, sizeof(uint32_t));
	retval->num_threads = num_threads;

	return retval;
}

/* blindingly obvious */
void barrier_free(barrier_t * barrier)
{
	free(barrier->arrive);
	free(barrier);
}

/* wait until NUM_THREADS threads have called this function concurrently on
 * the same barrier (i.e. have "arrived at" the barrier. 
 * The algorithm implemented here is described in "Foundations of multithreaded,
 * parallel, and distributed programming" by Gregory R Andrews. It is really 
 * rather straight-forward: every thread synchronizes with every other thread
 * either directly or indirectly. It does so by checking that the thread it
 * synchonizes with has arrived at the barrier and is at least in the same
 * stage of synchonization as itself. The barrier descriptor therefore has 
 * four fields that are important: the index of the next arriving thread 
 * (starts at 0, goes up to num_threads - 1); a shared array of integers which
 * denote the stage the thread is currently in (starting at 1, going up to 
 * an arbitary number, which is the number of stages multiplied by the number
 * of times the threads have all passed the barrier); the total number of 
 * stages (ceil(lg(num_threads))) and the total number of threads. 
 * To start, it finds out what its index number is and increments the index
 * number for the next arriving thread in a single, atomic action. It then
 * starts looping through the stages. In each stage, the thread increments
 * the number of the stage it is in and waits for the elected neighbour to 
 * catch up with him. It then doubles the distance to the neighbour and goes
 * through the same process again until it has, directly or indirectly, 
 * synchronized with all neighbours. Note that the last arriving process 
 * sets the index back to 0. This is so that the index values on the next
 * iteration re-start at 0. 
 * Possible optimizations would include avoiding memory contention on obtaining
 * the thread's index - e.g. by storing it in a thread-local variable and 
 * not re-calculating it when it's already known. I (rlc) don't really think
 * it's worth the trouble, though.. */
void barrier_wait(barrier_t * barrier)
{
	uint32_t index = 0;
	uint32_t stage;
	uint32_t neighbour_offs = 1;

	// get the index of the arriving process
	while (compare_and_exchange_int(&index, &(barrier->index), (index + 1)));
	// if we're the last one to arrive, we set the index back to zero for 
	// next time
	if (index == barrier->num_threads - 1)
		barrier->index =  0;
	// go through the necessary stages to assure every other process is there
	for (stage = 0; stage < barrier->stages; stage++)
	{
		atomic_increment((uint32_t*)&(barrier->arrive[index]));
		neighbour_offs = pow2(stage);
		while (barrier->arrive[(index + neighbour_offs) % barrier->num_threads])
			lt_thread_interrupt();
	}
}

