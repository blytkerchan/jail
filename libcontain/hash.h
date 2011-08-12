/* Jail: Just Another Interpreted Language
 * Copyright (c) 2003-2004, Ronald Landheer-Cieslak
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
#ifndef _HASH_H
#define _HASH_H

#include <sys/types.h>
#include "types.h"
#include "list_node.h"

#define HASH_DEFAULT_BUCKETS 7

typedef struct _hash_t
{
	list_node_t ** buckets;
	size_t n_buckets;
	uint32_t n_entries; // hint value only
	libcontain_cmp_func_t cmp;
	libcontain_hash_func_t hash;
} hash_t;

hash_t * hash_new(
	size_t n_buckets,
	libcontain_hash_func_t hash_func,
	libcontain_cmp_func_t compare_func
);
void hash_free(hash_t * hash);
void *hash_get(hash_t * hash, void *key);
void hash_put(hash_t * hash, void *key, void *value);
int hash_remove(hash_t * hash, void *key);
void ** hash_keys(hash_t * hash);

/* search the hash by value */
void * hash_search(hash_t * hash, void * searchfor, libcontain_cmp_func_t compare);
void hash_foreach(hash_t * hash, libcontain_foreach2_func_t func, void * data);

#endif // _HASH_H
