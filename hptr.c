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

extern smr_global_data_t * smr_global_data;
hptr_global_data_t * hptr_global_data = NULL;

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
		retval = (hptr_local_data_t*)calloc(1, sizeof(hptr_local_data_t));
		pthread_setspecific(hptr_global_data->key, retval);
		retval->hp = (void**)calloc(smr_global_data->k, sizeof(void*));
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
			free(curr);
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
