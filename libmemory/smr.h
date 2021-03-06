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
#ifndef _LIBMEMORY_SMR_H
#define _LIBMEMORY_SMR_H
/* This is an implementation of M.M. Micheal's Safe memory Reclamation 
 * memory management method. */
#ifndef DONT_HAVE_POSIX_THREADS
#include <pthread.h>
#endif
#if BUILDING_FOR_WOE32
#include <windows.h> // for TLS and DWORD
#endif
#include "hptr.h"

typedef void * (*smr_alloc_func_t)(size_t size);
typedef void (*smr_dealloc_func_t)(void *);

typedef struct _smr_global_data_t
{
	smr_alloc_func_t alloc_func;
	smr_dealloc_func_t dealloc_func;

	unsigned int p;		// number of participating threads
	unsigned int k;		// number of hazard pointers per thread

#ifndef DONT_HAVE_POSIX_THREADS
	pthread_key_t key;	// the key used for the thread-local data
#endif
#if BUILDING_FOR_WOE32
	DWORD key;
#endif

	hptr_local_data_t * first;
	hptr_local_data_t * last;
	hptr_local_data_t * free;
} smr_global_data_t;

typedef struct _smr_dlist_entry_t
{
	smr_dealloc_func_t dealloc_func;
	void * ptr;
} smr_dlist_entry_t;

typedef struct _smr_private_data_t
{
	unsigned int dcount;	// the number of pointers queued for destruction
	smr_dlist_entry_t * dlist;		// the pointers queued for destruction
	unsigned int dsize;	// size of the dlist in number of pointers
} smr_private_data_t;

/* Initialize and configure the SMR implementation */
int smr_init(smr_alloc_func_t alloc_func, smr_dealloc_func_t dealloc_func, unsigned int n_hptr);

/* Call this function at the start of every thread (or suffer the consequences)
 */
int smr_thread_init(void);

/* Call this if a thread reaches its end "naturally" (i.e. is not cancelled or
 * exited from) */
void smr_thread_fini(void);

/* Finalize the library's use - free all memory used by the library itself */
void smr_fini(void);

/* Allocate a chunk of SIZE bytes */
void * smr_malloc(size_t size);

/* Register a chunk of memory for use with SMR */
void smr_register(void * ptr);

/* Free a chunk of memory, safely */
void smr_free(void * ptr);

/* Free a chunk of memory safely, using a given function to do it when the time comes to do the actual de-allocation */
void smr_dealloc(smr_dealloc_func_t dealloc_func, void * ptr);

#ifndef SMR_NO_REPLACE
#define free smr_free
#define malloc smr_malloc
#endif // ! SMR_NO_REPLACE

#endif // _LIBMEMORY_SMR_H
