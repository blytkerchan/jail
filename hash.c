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
#include <libmemory/smr.h>
#include <libmemory/hptr.h>

#include "hash.h"

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
	retval->buckets = (list_t**)calloc(n_buckets, sizeof(list_t*));
	for (i = 0; i < n_buckets; i++)
		retval->buckets[i] = list_new(hash_node_cmp);
	retval->cmp = compare_func;
	retval->hash = hash_func;
	retval->n_buckets = n_buckets;

	return retval;
}

void hash_free_helper(const void * val, void * data)
{
	hash_node_free((hash_node_t*)val);
}

void hash_free(hash_t * hash)
{
	int i;
	
	for (i = 0; i < hash->n_buckets; i++)
	{
		list_foreach(hash->buckets[i], hash_free_helper, NULL);
		list_free(hash->buckets[i]);
	}
	free(hash);
}

void *hash_get(hash_t * hash, void *key)
{
	void * retval;
	unsigned int _hash;
	list_t * list;
	hash_node_t * node;
	hash_node_t * rnode;

	_hash = hash->hash(key);
	list = hash->buckets[_hash % hash->n_buckets];
	node = hash_node_new();
	node->key = key;
	/* the following searches the list twice. I don't especially like that but
	 * in this setup we need a hazardous reference to the hash node, which 
	 * means that once we've registered it, we need to know it is still OK */
	while ((rnode = list_search(list, node)) != hptr_get(3))
	{
		hptr_register(3, rnode);
	} 
	if (rnode)
		retval = rnode->val;
	else
		retval = NULL;
	hptr_free(3);
	hash_node_free(node);
	
	return retval;
}

int hash_put(hash_t * hash, void *key, void *value)
{
	return -1;
}

int hash_remove(hash_t * hash, void *key)
{
	return -1;
}

void ** hash_keys(hash_t * hash)
{
	return NULL;
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

void hash_foreach(hash_t * hash, libcontain_foreach2_func_t func, void * data)
{
}
