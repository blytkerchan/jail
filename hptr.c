/* Jail: Just Another Interpreted Language
 * Copyright (c) 2003, Ronald Landheer-Cieslak
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
#define SMR_NO_REPLACE
#include "smr.h"
#include "hptr.h"

#include <stdlib.h>

#include "arch/include/compare_and_exchange.h"
#include "arch/include/set.h"

extern smr_global_data_t * smr_global_data;
hptr_global_data_t * hptr_global_data = NULL;

// we assume to have exclusive ownership of DATA: we're the only ones allowed
// to write to it as the owning thread is (normally) dying.
// This assumption should remain valid until the moment we put the node (data)
// in the place of the "free" field of the SMR global data, at which time
// we relinquish ownership.
// A thread dequeueing a node from free should not assume to have ownership until
// it succeeds a CAS setting the new value of the free field in the SMR global data.
// That's why we set that field to NULL to assume ownership of the node as well. 
// Normally, no-one should be traversing the queue of nodes at that time, but 
// we take some measures to accomodate for that (im)possibility anyway.
// Also note we've added a flag to the local data type (hptr_local_data_t)
// which is 1 if the hptr scanning logic should no longer count on our 
// "next" field to be valid.
static void smr_hptr_free_list_enq(hptr_local_data_t * data)
{
	int i;
	hptr_local_data_t * exp;
	hptr_local_data_t * ptr = NULL;
	
	// make our node ready for inclusion in the free list
	// clear out all existing hazardous references
	for (i = 0; i < smr_global_data->k; i++)
		atomic_set_ptr(&(data->hp[i]), NULL);
	// set out flag
	atomic_set_int(&(data->flag), 1);
	// NOTE: we expect atomic_set_* to do the right thing concerning 
	// memory barriers: we expect the setting of the flag to be visible 
	// before the clearing of the next pointer!
	
	// clear out our next pointer
	atomic_set_ptr(&(data->next), NULL);
	// try to set it to smr_global_data->free first.
	exp = NULL;
	while (compare_and_exchange_ptr(&exp, &(smr_global_data->free), data) != 0)
	{	// Otherwise, set smr_global_data->free to NULL (and get whatever is in there);
		// set its old value to our next and put us in its place.
		// "Whatever is in there" is what we have in exp now..
		if (compare_and_exchange(&exp, &(smr_global_data->free), NULL) != 0)
			continue;
		// we may have been here more than once, in which case ptr holds whatever we have in our next field..
		if (compare_and_exchange(&ptr, &(data->next), NULL) != 0)
			assert(0); // just in case..
		// we now put whatever is in ptr at the end of exp - which might be a whole queue for all we know..
		if (exp != NULL && ptr != NULL) 
		{
			hptr_local_data_t * head = exp;
			hptr_local_data_t * tail = exp;
			
			while (1)
			{
				// find the tail of the queue in exp (now head)
				while (tail->next != NULL)
					tail = tail->next;
				// we expect a NULL at the end of this queue
				exp = NULL;
				// try to put ptr at the end of the queue
				if (compare_and_exchange_ptr(&exp, &(tail->next), ptr) != 0)
				{	// otherwise, we start over from scratch
					tail = head;
					continue;
				}
			}
			// when we get here, ptr is at the end of the queue we want to add to our node
			// so we add the queue...
			if (compare_and_exchange(&exp, &(data->next), head) != 0)
				// someone was still writing to our node - that should be impossible..
				assert(0);
		}
	}
	// when we get here, we're done.
}

// the logic here should be much simpler that that of smr_hptr_free_list_enq: we
// simply try to obtain a node from the free list and keep trying until our CAS
// works.
static hptr_local_data_t * smr_hptr_free_list_deq(void)
{
	hptr_local_data_t * retval;
	hptr_local_data_t * next;
	
	do
	{
		retval = smr_global_data->free;
		next = retval->next;
	} while (compare_and_exchange_ptr(&retval, &(smr_global_data->free), next) != 0)

	// we now have ownership of retval.
	retval->next = NULL;
	retval->flag = 0;
	
	return retval;
}

static void hptr_register_local_data(hptr_local_data_t * data)
{
	void * exp;
	int registered = 0;
	
try_again:
	if (smr_global_data->first == NULL)
	{
		exp = NULL;
		if (compare_and_exchange_ptr(&exp, &(smr_global_data->first), data) != 0)
			goto try_again;
		registered = 1;
	}
	if (smr_global_data->last == NULL)
	{
		exp = NULL;
		if (compare_and_exchange_ptr(&exp, &(smr_global_data->last), smr_global_data->first) != 0)
			goto try_again;
	}
	if (registered)
		return;
	while (smr_global_data->last->next != NULL)
	{
		exp = smr_global_data->last;
		compare_and_exchange_ptr(&exp, &(smr_global_data->last), smr_global_data->last->next);
	}
	data->next = NULL;
	exp = NULL;
	if (compare_and_exchange_ptr(&exp, &(smr_global_data->last->next), data))
		goto try_again;
	exp = smr_global_data->last;
	compare_and_exchange_ptr(&exp, &(smr_global_data->last), data);
}

static hptr_local_data_t * hptr_get_local_data(void)
{
	hptr_local_data_t * retval;

	retval = pthread_getspecific(hptr_global_data->key);
	if (retval == NULL)
	{
		if ((retval = smr_hptr_free_list_deq()) == NULL)
		{
			retval = (hptr_local_data_t*)calloc(1, sizeof(hptr_local_data_t));
			pthread_setspecific(hptr_global_data->key, retval);
			retval->hp = (void**)calloc(smr_global_data->k, sizeof(void*));
		}
		hptr_register_local_data(retval);
	}

	return retval;
}

static void hptr_cleanup_local_data(void * data)
{
	hptr_local_data_t * curr;
	hptr_local_data_t * prev;
	
	if (data == NULL)
		return;
try_again:
	curr = smr_global_data->first;
	prev = NULL;
	while (curr != NULL)
	{
		if (curr == data)
		{
			if (prev == NULL)
			{
				if (compare_and_exchange_ptr(&curr, &(smr_global_data->first), curr->next))
					goto try_again;
			}
			else
			{
				if (compare_and_exchange_ptr(&curr, &(prev->next), curr->next))
					goto try_again;
			}
			smr_hptr_free_list_enq(curr);
			return;
		}

		prev = curr;
		curr = curr->next;
	}
}

int hptr_init(void)
{
	hptr_global_data = (hptr_global_data_t *)calloc(1, sizeof(hptr_global_data_t));
	pthread_key_create(&(hptr_global_data->key), hptr_cleanup_local_data);

	return(0);
}

void hptr_fini(void)
{
	free(hptr_global_data);
}

int hptr_register(unsigned int index, void * ptr)
{
	void * exp = NULL;
	hptr_local_data_t * local_data;
	
	local_data = hptr_get_local_data();
	while (compare_and_exchange_ptr(&exp, &(local_data->hp[index]), ptr));

	return(0);
}

void hptr_free(unsigned int index)
{
	void * exp = NULL;
	hptr_local_data_t * local_data;

	local_data = hptr_get_local_data();
	while (compare_and_exchange_ptr(&exp, &(local_data->hp[index]), NULL));
}

void * hptr_get(unsigned int index)
{
	void * retval;
	hptr_local_data_t * local_data;

	local_data = hptr_get_local_data();
	retval = local_data->hp[index];

	return retval;
}
