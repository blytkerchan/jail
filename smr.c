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
#ifndef DONT_HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include "arch/increment.h"
#include "arch/decrement.h"
#include "arch/membar.h"

smr_global_data_t * smr_global_data = NULL;

// forward declaration
static void smr_scan_worker(smr_private_data_t * priv_data);

static smr_private_data_t * smr_get_private_data(void)
{
	smr_private_data_t * retval;
	
#ifndef DONT_HAVE_POSIX_THREADS
	retval = pthread_getspecific(smr_global_data->key);
#endif
#if BUILDING_FOR_WOE32
	retval = TlsGetValue(smr_global_data->key);
#endif
	if (retval == NULL)
	{
		retval = (smr_private_data_t*)calloc(1, sizeof(smr_private_data_t));
#ifndef DONT_HAVE_POSIX_THREADS
		pthread_setspecific(smr_global_data->key, retval);
#endif
#if BUILDING_FOR_WOE32
		TlsSetValue(smr_global_data->key, retval);
#endif
		retval->dcount = 0;
		retval->dsize = (smr_global_data->k * smr_global_data->p) + 1;
		retval->dlist = calloc(retval->dsize, sizeof(smr_dlist_entry_t));
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
#ifndef DONT_HAVE_STDSLEEP
			sleep(0);
#endif
#if BUILDING_FOR_WOE32
			Sleep(1);
#endif
		}
	}
	free(priv->dlist);
	free(data);
}

int smr_init(smr_alloc_func_t alloc_func, smr_dealloc_func_t dealloc_func, unsigned int n_hptr)
{
	int rv;
	
	smr_global_data = (smr_global_data_t*)calloc(1, sizeof(smr_global_data_t));
	if (alloc_func)
        smr_global_data->alloc_func = alloc_func;
	else
		smr_global_data->alloc_func = malloc;
	if (dealloc_func)
		smr_global_data->dealloc_func = dealloc_func;
	else
		smr_global_data->dealloc_func = free;
	smr_global_data->p = 1;
	smr_global_data->k = n_hptr;  
#ifndef DONT_HAVE_POSIX_THREADS
	rv = pthread_key_create(&(smr_global_data->key), smr_cleanup_private_data);
#endif
#if BUILDING_FOR_WOE32
	/* Windoze doesn't have the possibility to have a "destructor" called on 
	* thread exit. If we're in a DLL, the DLL can take care of this for us. 
	* If not, too bad! */
	smr_global_data->key = TlsAlloc();
	rv = 0;
#endif
	
	return(rv ? -1 : 0);
}

void smr_fini(void)
{
#ifndef DONT_HAVE_POSIX_THREADS
	pthread_key_delete(smr_global_data->key);
#endif
#if BUILDING_FOR_WOE32
	TlsFree(smr_global_data->key);
#endif
	free(smr_global_data);
}

void * smr_malloc(size_t size)
{
	if (size > 0)
		return smr_global_data->alloc_func(size);
	else
		return smr_global_data->alloc_func(1);
}

void smr_register(void * ptr)
{ /* no-op: nothing to do here */ }


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
	void ** plist = NULL;
	smr_dlist_entry_t * new_dlist;
	hptr_local_data_t * hptr_data;
	hptr_local_data_t * next;
	unsigned int R;
	unsigned int new_dsize;

smr_scan_worker_start:
	R = smr_global_data->k * smr_global_data->p;
	new_dsize = R + 1;
	p = new_dcount = 0;
	plist = (void**)realloc(plist, R * sizeof(void*));
	memset(plist, 0, R * sizeof(void*));
	new_dlist = calloc(new_dsize, sizeof(smr_dlist_entry_t));
		
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
		next = hptr_data->next;
		membar_rw();
		// having a set flag here means we can't count on the next pointer being OK, 
		// which means the hazard pointers we've read from this node are tainted, 
		// which means we have to start over :(
		if (hptr_data->flag)
			goto smr_scan_worker_start;
		hptr_data = next;
	}
	
	// stage 2
	qsort(plist, p, sizeof(void *), smr_qsort_helper);
	
	// stage 3
	for (i = 0; i < priv_data->dsize; i++)
	{
		if (smr_binary_search(plist, p, priv_data->dlist[i].ptr) != ~0)
		{
			if (priv_data->dlist[i].ptr) // don't re-queue NULL
			{
				new_dlist[new_dcount++].dealloc_func = priv_data->dlist[i].dealloc_func;
				new_dlist[new_dcount++].ptr = priv_data->dlist[i].ptr;
			}
		}
		else if (priv_data->dlist[i].dealloc_func)
		{
			priv_data->dlist[i].dealloc_func(priv_data->dlist[i].ptr);
			priv_data->dlist[i].ptr = 0;
			priv_data->dlist[i].dealloc_func = 0;
		}
		else
		{ /* nothing to do: there is no de-allocator function and the 
		   * pointer wasn't found in the list of pointers to which 
		   * there are still hazardous references. If the pointer points 
		   * anything, there is no way for us to free that memory
		   * safely.. */ }
	}
	
	// stage 4
	free(priv_data->dlist);
	priv_data->dlist = new_dlist;
	priv_data->dsize = new_dsize;
	priv_data->dcount = new_dcount;
	
	free(plist);
}

void smr_scan(void)
{
	smr_private_data_t * priv_data;

	priv_data = smr_get_private_data();
	smr_scan_worker(priv_data);
}


void smr_free(void * ptr)
{
	smr_dealloc(smr_global_data->dealloc_func, ptr);
}

void smr_dealloc(smr_dealloc_func_t dealloc_func, void * ptr)
{
	smr_private_data_t * priv_data;
	priv_data = smr_get_private_data();
	priv_data->dlist[priv_data->dcount].dealloc_func = dealloc_func;
	priv_data->dlist[priv_data->dcount].ptr = ptr;
	priv_data->dcount++;
	if (priv_data->dcount >= priv_data->dsize)
		smr_scan();
}

static void smr_thread_cleanup(void)
{
	smr_private_data_t * priv;

#ifndef DONT_HAVE_POSIX_THREADS
	priv = pthread_getspecific(smr_global_data->key);
#endif
#if BUILDING_FOR_WOE32
	priv = TlsGetValue(smr_global_data->key);
#endif
	if (priv != NULL)
	{
		smr_cleanup_private_data(priv);
	}
#ifndef DONT_HAVE_POSIX_THREADS
	pthread_setspecific(smr_global_data->key, NULL);
#endif
#if BUILDING_FOR_WOE32
	TlsSetValue(smr_global_data->key, NULL);
#endif
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

