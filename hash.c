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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hash.h"
#include "c_interface.h"
#if ! DONT_USE_GLIB
#include "glib/ghash.h"
#endif // DONT_USE_GLIB

hash_t * new_hash(libhash_hashtype hash_type,
						hash_key_hash_func_t hash_func,
						hash_key_cmp_func_t compare_func)
				    {
	hash_t * retval = (hash_t*)malloc(sizeof(hash_t));
	memset(retval, 0, sizeof(hash_t));
	
	switch (hash_type)
	{
#if ! DONT_USE_CXX
	case NORMAL_HASH :
	case STRING_HASH :
	case INT_HASH :
		retval->cxx_hash = cxx_new_hash(hash_type);
		return retval;
#endif
#if ! DONT_USE_GLIB
	case GLIB_HASH :
		retval->glib_hash = g_hash_table_new(hash_func, compare_func);
		return retval;
#endif
	default :
		return NULL;
	}
}

void delete_hash(hash_t * hash)
{
#if ! DONT_USE_GLIB
	if (hash->glib_hash)
		g_hash_table_destroy(hash->glib_hash);
#endif 
#if ! DONT_USE_CXX
	if (hash->cxx_hash)
		cxx_delete_hash(hash->cxx_hash);
#endif
	free(hash);
}

void *hash_get(hash_t * hash, const void *key)
{
#if ! DONT_USE_GLIB
	if (hash->glib_hash)
		return g_hash_table_lookup(hash->glib_hash, key);
#endif
#if ! DONT_USE_CXX
	if (hash->cxx_hash)
		return cxx_hash_get(hash->cxx_hash, key);
#endif 
	return NULL;
}

int hash_put(hash_t * hash, const void *key, const void *value)
{
#if ! DONT_USE_GLIB
	if (hash->glib_hash)
	{
		g_hash_table_insert(hash->glib_hash, (void*)key, (void*)value);
		return 0;
	}
#endif 
#if ! DONT_USE_CXX
	if (hash->cxx_hash)
		return cxx_hash_put(hash->cxx_hash, key, value);
#endif
	return -1;
}

int hash_remove(hash_t * hash, const void *key)
{
#if ! DONT_USE_GLIB
	if (hash->glib_hash)
	{
		g_hash_table_remove(hash->glib_hash, key);
		return 0;
	}
#endif
#if ! DONT_USE_CXX
	if (hash->cxx_hash)
		return cxx_hash_remove(hash->cxx_hash, key);
#endif
	return -1;
}

#if ! DONT_USE_GLIB
typedef struct _key_container_t
{
	void ** keys;
	unsigned int n_keys;
} key_container_t;

void hash_keys_helper(gpointer key, gpointer value, gpointer user_data)
{
	key_container_t * val = (key_container_t*)user_data;

	val->keys[val->n_keys] = strdup((char*)key);
	val->n_keys++;
}
#endif 
void ** hash_keys(hash_t * hash)
{
#if ! DONT_USE_GLIB
	if (hash->glib_hash)
	{
		void ** retval = (void**)malloc(sizeof(void*) * g_hash_table_size(hash->glib_hash));
		key_container_t * tval = (key_container_t*)malloc(sizeof(key_container_t));
		memset(tval, 0, sizeof(key_container_t));
		tval->keys = retval;
		g_hash_table_foreach(hash->glib_hash, hash_keys_helper, tval);
		return retval;
	}
#endif
#if ! DONT_USE_CXX
	if (hash->cxx_hash)
		return cxx_hash_keys(hash->cxx_hash);
#endif
	return NULL;
}

struct hash_search_t
{
	hash_val_cmp_func_t compare;
	void * searchfor;
	void * retval;
};

static void hash_search_helper(void * key, void * val, void * data)
{
	struct hash_search_t * s_data = (struct hash_search_t*)data;
	if (s_data->compare(val, s_data->searchfor) == 0)
	{
		s_data->retval = val;
	}
}

void * hash_search(hash_t * hash, void * searchfor, hash_val_cmp_func_t compare)
{
	
	struct hash_search_t hash_search_data;
	hash_search_data.retval = NULL;
	hash_search_data.searchfor = searchfor;
	hash_search_data.compare = compare;
	hash_foreach(hash, hash_search_helper, &hash_search_data);
	
	return hash_search_data.retval;
}

void hash_foreach(hash_t * hash, hash_foreach_helper_func_t func, void * data)
{
#if ! DONT_USE_GLIB
	if (hash->glib_hash)
	{
		g_hash_table_foreach(hash->glib_hash, func, data);
		return;
	}
#endif
#if ! DONT_USE_CXX
	if (hash->cxx_hash)
	{
		hash->cxx_hash->for_each(func, data);
		return;
	}
#endif
}
