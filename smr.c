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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "arch/include/increment.h"
#include "arch/include/decrement.h"

smr_global_data_t * smr_global_data = NULL;

// forward declaration
static void smr_scan_worker(smr_private_data_t * priv_data);

static smr_private_data_t * smr_get_private_data(void)
{
	smr_private_data_t * retval;
	
	retval = pthread_getspecific(smr_global_data->key);
	if (retval == NULL)
	{
		retval = (smr_private_data_t*)calloc(1, sizeof(smr_private_data_t));
		pthread_setspecific(smr_global_data->key, retval);
		retval->dcount = 0;
		retval->dsize = (smr_global_data->k * smr_global_data->p) + 1;
		retval->dlist = (void**)calloc(retval->dsize, sizeof(void*));
	}
	
	return retval;
}

/* FIXME: this could wait forever if the local hazard pointers aren't cleared or
 * if clearing hazard pointers from other threads depends on this one exiting.
 * I don't particularly like that.. */
static void smr_cleanup_private_data(void * data)
{
	smr_private_data_t * priv = (smr_private_data_t*)data;
	
	if (data == NULL)
		return;
	while (priv->dcount)
	{
		// There are pointers still queued for deletion - try to delete them now.
		smr_scan_worker(priv);
		if (priv->dcount)
		{
			// still queued for deletion - we'll have to wait a bit..
			sleep(0);
		}
	}
	free(priv->dlist);
	free(data);
}

int smr_init(unsigned int n_hptr)
{
	int rv;
	
	smr_global_data = (smr_global_data_t*)calloc(1, sizeof(smr_global_data_t));
	smr_global_data->p = 1;
	smr_global_data->k = n_hptr;  
	rv = pthread_key_create(&(smr_global_data->key), smr_cleanup_private_data);
	
	return(rv ? -1 : 0);
}

void smr_fini(void)
{
	pthread_key_delete(smr_global_data->key);
	free(smr_global_data);
}

/* This version of malloc has nothing to do with SMR, but fixes a common problem
 * with native malloc() implementations */
void * smr_malloc(size_t size)
{
	if (size > 0)
		return malloc(size);
	else
		return malloc(1);
}

static int smr_qsort_helper(const void * ptr1, const void * ptr2)
{
	return(*((int*)ptr2) - *((int*)ptr1));
}

static unsigned int smr_binary_search_worker(void ** list, size_t lower, size_t upper, void * ptr)
{
	unsigned int mid;
	
	if (lower == upper)
	{
		if (list[lower] == ptr)
			return lower;
		else
			return ~0;
	}
	mid = (lower + upper) / 2;
	if (((int)ptr) > ((int)list[mid]))
	{
		if (mid == upper)
			return ~0;
		return smr_binary_search_worker(list, mid + 1, upper, ptr);
	}
	else
	{
		if (((int)ptr) < ((int)list[mid]))
		{
			if (mid == lower)
				return ~0;
			return smr_binary_search_worker(list, lower, mid - 1, ptr);
		}
		else
			return mid;
	}
}

static unsigned int smr_binary_search(void ** list, size_t size, void * ptr)
{
	return smr_binary_search_worker(list, 0, size, ptr);
}

/* FIXME: if during (or before) stage 1, the number of threads is increased,
 *        there will not be enough space in the plist for all the hazard 
 *        pointers. There are to ways to handle this: loop if P changes, or
 *        lock during stage 1 (i.e. wait or block). */
static void smr_scan_worker(smr_private_data_t * priv_data)
{
	unsigned int i, p, new_dcount;
	void * hptr;
	void ** plist;
	void ** new_dlist;
	hptr_local_data_t * hptr_data;
	unsigned int R;
	unsigned int new_dsize;

smr_scan_worker_start:
	R = smr_global_data->k * smr_global_data->p;
	new_dsize = R + 1;
	p = new_dcount = 0;
	plist = (void**)alloca(R * sizeof(void*));
	memset(plist, 0, R * sizeof(void*));
	new_dlist = (void**)calloc(new_dsize, sizeof(void*));
		
	// stage 1
	hptr_data = smr_global_data->first;
	while (hptr_data != NULL)
	{
		for (i = 0; i < smr_global_data->k; i++)
		{
			if ((hptr = hptr_data->hp[i]) != NULL)
				plist[p++] = hptr;
			if (p == R)
			{
				free(new_dlist);
				goto smr_scan_worker_start;
			}
		}
		hptr_data = hptr_data->next;
	}
	
	// stage 2
	qsort(plist, p, sizeof(void *), smr_qsort_helper);
	
	// stage 3
	for (i = 0; i < priv_data->dsize; i++)
	{
		if (smr_binary_search(plist, p, priv_data->dlist[i]) != ~0)
		{
			if (priv_data->dlist[i]) // don't re-queue NULL
				new_dlist[new_dcount++] = priv_data->dlist[i];
		}
		else
			free(priv_data->dlist[i]);
	}
	
	// stage 4
	free(priv_data->dlist);
	priv_data->dlist = new_dlist;
	priv_data->dsize = new_dsize;
	priv_data->dcount = new_dcount;
}

void smr_scan(void)
{
	smr_private_data_t * priv_data;

	priv_data = smr_get_private_data();
	smr_scan_worker(priv_data);
}


void smr_free(void * ptr)
{
	smr_private_data_t * priv_data;
	priv_data = smr_get_private_data();
	priv_data->dlist[priv_data->dcount++] = ptr;
	if (priv_data->dcount >= priv_data->dsize)
		smr_scan();
}

static void smr_thread_cleanup(void)
{
	smr_private_data_t * priv;

	priv = pthread_getspecific(smr_global_data->key);
	if (priv != NULL)
	{
		smr_cleanup_private_data(priv);
	}
	pthread_setspecific(smr_global_data->key, NULL);
}

int smr_thread_init(void)
{
	atomic_increment((uint32_t*)&(smr_global_data->p));

	return 0;
}

void smr_thread_fini(void)
{
	smr_thread_cleanup();
	atomic_decrement((uint32_t*)&(smr_global_data->p));
}

