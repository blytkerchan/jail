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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "libmemory/smr.h"
#include "libmemory/hptr.h"
#include "arch/include/compare_and_exchange.h"
#include "arch/include/increment.h"
#include "hash.h"

typedef struct _hash_node_t
{
	unsigned int hash;
	void * key;
	void * val;
	libcontain_cmp_func_t cmp;
} hash_node_t;

hash_node_t * hash_node_new(void)
{
	hash_node_t * retval = calloc(1, sizeof(hash_node_t));

	return retval;
}

void hash_node_free(hash_node_t * hash_node)
{
	free(hash_node);
}

int hash_node_cmp(const void * node1, const void * node2)
{
	const hash_node_t * hash_node1 = (const hash_node_t *)node1;
	const hash_node_t * hash_node2 = (const hash_node_t *)node2;
	int rc;
	
	if ((rc = hash_node1->hash - hash_node2->hash) == 0)
		return hash_node1->cmp(hash_node1->key, hash_node2->key);
	else return rc;
}

hash_t * hash_new(
	size_t n_buckets,
	libcontain_hash_func_t hash_func,
	libcontain_cmp_func_t compare_func
)
{
	int i;
	hash_t * retval = (hash_t*)calloc(1, sizeof(hash_t));

	if (n_buckets == 0)
		n_buckets = HASH_DEFAULT_BUCKETS;
	retval->buckets = calloc(n_buckets, sizeof(list_node_t*));
	for (i = 0; i < n_buckets; i++)
		retval->buckets[i] = list_node_new(NULL);
	retval->cmp = compare_func;
	retval->hash = hash_func;
	retval->n_buckets = n_buckets;

	return retval;
}

void hash_free(hash_t * hash)
{
	int i;
	
	for (i = 0; i < hash->n_buckets; i++)
	{
		list_node_t * curr;
		list_node_t * next;

		curr = hash->buckets[i];
		while (curr)
		{
			next = curr->next;
			hash_node_free((hash_node_t*)curr->val);
			curr = next;
		}
	}
	free(hash);
}

void * hash_get(hash_t * hash, void *key)
{
	void * retval;
	unsigned int _hash;
	list_node_t * list_node;
	hash_node_t * temp_node;
	hash_node_t * ret_node;
	list_state_t list_state;
	
	memset(&list_state, 0, sizeof(list_state_t));
	temp_node = hash_node_new();
	temp_node->key = key;
	_hash = hash->hash(key);
	list_node = hash->buckets[_hash % hash->n_buckets];
	list_node = list_node_find(&list_state, list_node, hash_node_cmp, temp_node);
	do
	{
		ret_node = list_node->val;
		hptr_register(0, ret_node);
	} while (ret_node != list_node->val);
	hptr_free(1);
	hptr_free(2);
	if (ret_node)
		retval = ret_node->val;
	else
		retval = NULL;
	hptr_free(0);
	hash_node_free(temp_node);
	
	return retval;
}

void hash_put(hash_t * hash, void *key, void *value)
{
	unsigned int _hash;
	list_node_t * list_node;
	list_node_t * new_node;
	list_state_t list_state;
	hash_node_t * hash_node;
	hash_node_t * old_node;
	
	memset(&list_state, 0, sizeof(list_state_t));
	hash_node = hash_node_new();
	_hash = hash->hash(key);
	list_node = hash->buckets[_hash % hash->n_buckets];
	new_node = NULL;
	while (1)
	{
		new_node = list_node_find(&list_state, list_node, hash_node_cmp, hash_node);
		if (new_node == NULL)
			new_node = list_node_new(hash_node);
		else
		{
			old_node = new_node->val;
			if (compare_and_exchange_ptr(&old_node, &(new_node->val), hash_node) == 0)
			{
				hash_node_free(old_node);
				break;
			}
		}
		if (list_node_insert(list_node, hash_node_cmp, new_node) == 0)
			break;
		list_node_free(new_node);
	}

	atomic_increment(&(hash->n_entries));
}

int hash_remove(hash_t * hash, void *key)
{
	list_node_t * list_node;
	unsigned int _hash;
	list_state_t list_state;
	hash_node_t hash_node;
	
	memset(&hash_node, 0, sizeof(hash_node_t));
	hash_node.key = key;
	_hash = hash->hash(key);
	while (1)
	{
		memset(&list_state, 0, sizeof(list_state_t));
		list_node = list_node_find(&list_state, hash->buckets[_hash % hash->n_buckets], hash_node_cmp, &hash_node);
		if (list_node == NULL)
		{
			hptr_free(0);
			hptr_free(1);
			hptr_free(2);
			return -1;
		}
		if (compare_and_exchange_ptr(&list_node, &(list_state.prev->next), list_state.next) == 0)
			break;
	}
	hptr_free(0);
	hptr_free(1);
	hptr_free(2);

	return 0;
}

void ** hash_keys(hash_t * hash)
{
	void ** retval;
	uint32_t allocated = hash->n_entries + 1;
	uint32_t index = 0;
	uint32_t bucket;
	
	/* use the hint value to allocate the first array of keys */
	retval = calloc(allocated, sizeof(void*));
	for (bucket = 0; bucket < hash->n_buckets; bucket++)
	{
		/* as in list_node_find, hptr0 contains prev; hptr1 contains
		 * curr; hptr2 contains next. Also: hptr3 contains the hash
		 * node in curr */
		list_node_t * prev = NULL;
		list_node_t * curr;
		list_node_t * next;
		hash_node_t * hash_node;

		do
		{
			curr = hash->buckets[bucket];
			hptr_register(1, curr);
		} while (curr != hash->buckets[bucket]);
		while (1)
		{
			do
			{
				next = curr->next;
				hptr_register(2, next);
			} while (next != curr->next);
			prev = curr;
			hptr_register(0, prev);
			curr = next;
			if (curr == NULL)
			{
				hptr_free(0);
				hptr_free(1);
				hptr_free(2);
				hptr_free(3);
				break;
			}
			do
			{
				hash_node = curr->val;
				hptr_register(3, hash_node);
			} while (hash_node != curr->val);
			retval[index++] = hash_node->key;
			if (index == allocated)
			{
				allocated++;
				retval = realloc(retval, allocated * sizeof(void*));
			}
		}
	}
	retval[index] = NULL;
	
	return retval;
}

struct hash_search_t
{
	libcontain_cmp_func_t compare;
	void * searchfor;
	void * retval;
};

static void hash_search_helper(const void * key, const void * val, void * data)
{
	struct hash_search_t * s_data = (struct hash_search_t*)data;
	if (s_data->compare(val, s_data->searchfor) == 0)
	{
		s_data->retval = (void*)val;
	}
}

void * hash_search(hash_t * hash, void * searchfor, libcontain_cmp_func_t compare)
{
	
	struct hash_search_t hash_search_data;
	hash_search_data.retval = NULL;
	hash_search_data.searchfor = searchfor;
	hash_search_data.compare = compare;
	hash_foreach(hash, hash_search_helper, &hash_search_data);
	
	return hash_search_data.retval;
}

struct hash_foreach_helper_data_t
{
	libcontain_foreach2_func_t func;
	void * data;
};

static void hash_foreach_helper(const void * val, void * data)
{
	struct hash_foreach_helper_data_t * hash_foreach_helper_data = (struct hash_foreach_helper_data_t*)data;
	
	hash_node_t * node = (hash_node_t*)val;
	hash_foreach_helper_data->func(node->key, node->val, hash_foreach_helper_data->data);
}

void hash_foreach(hash_t * hash, libcontain_foreach2_func_t func, void * data)
{
	uint32_t bucket;
	struct hash_foreach_helper_data_t hash_foreach_helper_data;
	
	hash_foreach_helper_data.func = func;
	hash_foreach_helper_data.data = data;
	for (bucket = 0; hash->n_buckets; bucket++)
		list_node_foreach(hash->buckets[bucket], hash_foreach_helper, &hash_foreach_helper_data);
}

