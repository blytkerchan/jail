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
#include <stdlib.h> /* for NULL */
#include "arch/include/compare_and_exchange.h"
#include "binomial_tree.h"
#include "map.h"

/* forward declarations */
static map_node_t * map_node_new(void * key, void * val);
static void map_node_free(map_node_t * handle);

map_t * map_new(libcontain_cmp_func_t cmp_func)
{
	map_t * retval;

	retval = (map_t*)calloc(1, sizeof(map_t));
	retval->cmp_func = cmp_func;
	retval->tree = binomial_tree_new();

	return retval;
}

void map_free_helper(const void * dat1, void * dat2)
{
	map_node_free((map_node_t*)dat1);
}

void map_free(map_t * handle)
{
	binomial_tree_foreach(handle->tree, map_free_helper, NULL);
	binomial_tree_free(handle->tree);
	free(handle);
}

void map_insert(map_t * handle, void * key, void * val)
{
	binomial_tree_node_t * curr;
	map_node_t * cval;
	map_node_t * node = map_node_new(key, val);
	int rc;

	curr = binomial_tree_get_root(handle->tree);
	while (1)
	{
		cval = (map_node_t*)binomial_tree_node_get_value(curr);
		if (cval == NULL)
		{
			if (binomial_tree_node_set_value(curr, cval, node) == 0)
			{
				binomial_tree_node_release(curr);
				break;
			}
			continue;
		}
		if (cval->flag)
		{
			curr = binomial_tree_node_get_right(curr);
			continue;
		}
		rc = handle->cmp_func(node->key, cval->key);
		if (rc < 0)
		{
			curr = binomial_tree_node_get_left(curr);
		}
		else if (rc > 0)
		{
			curr = binomial_tree_node_get_right(curr);
		}
		else
		{
			if (binomial_tree_node_set_value(curr, cval, node) == 0)
			{
				binomial_tree_node_release(curr);
				break;
			}
		}
	}
}

void * map_get(map_t * handle, void * key)
{
	binomial_tree_node_t * curr;
	map_node_t * cval;
	int rc;

	curr = binomial_tree_get_root(handle->tree);
	while (1)
	{
		cval = (map_node_t*)binomial_tree_node_get_value(curr);
		if (cval == NULL)
		{
			binomial_tree_node_release(curr);
			return NULL;
		}
		if (cval->flag == 2)
		{
			curr = binomial_tree_node_get_right(curr);
			continue;
		}
		rc = handle->cmp_func(key, cval->key);
		if (rc < 0)
		{
			curr = binomial_tree_node_get_left(curr);
		}
		else if (rc > 0)
		{
			curr = binomial_tree_node_get_right(curr);
		}
		else
		{
			if (cval->flag)
			{
				curr = binomial_tree_node_get_right(curr);
			}
			else
			{
				binomial_tree_node_release(curr);
				return cval->val;
			}
		}
	}
}

typedef struct 
{
	map_t * handle;
	binomial_tree_node_t * dnode_right;
} map_remove_helper_data_t;

static void map_remove_helper(const void * val, void * data)
{
	binomial_tree_node_t * curr;
	map_node_t * cval;
	map_node_t * node = (map_node_t*)val;
	map_remove_helper_data_t * dat = (map_remove_helper_data_t*)data;
	int rc;

	curr = dat->dnode_right;
	while (1)
	{
		cval = (map_node_t*)binomial_tree_node_get_value(curr);
		if (cval == NULL)
		{
			if (binomial_tree_node_set_value(curr, cval, node) == 0)
			{
				binomial_tree_node_release(curr);
				break;
			}
			continue;
		}
		if (cval->flag)
		{
			curr = binomial_tree_node_get_right(curr);
			continue;
		}
		rc = dat->handle->cmp_func(node->key, cval->key);
		if (rc < 0)
		{
			curr = binomial_tree_node_get_left(curr);
		}
		else if (rc > 0)
		{
			curr = binomial_tree_node_get_right(curr);
		}
		else
		{
			if (binomial_tree_node_set_value(curr, cval, node) == 0)
			{
				binomial_tree_node_release(curr);
				break;
			}
		}
	}
}

/* removing a node is a three-step process:
 * 1. first, we find the node and tag it as deleted
 * 2. then, we walk the tree on its left hand side and add the
 *    nodes to the tree on the right-hand side
 * 3. then, we remove the node from the tree.
 * While the node is tagged as deleted, search operations will still traverse
 * it normally, except that when looking for the key of the deleted node itself,
 * the right-hand tree is taken and the value of the node itself is not 
 * returned. When adding a new node, the node can't be added to the left-hand
 * tree.
 * If a search for a node to remove traverses a node that is being removed, the
 * search must back up to the parent and try again..
 */
void map_remove(map_t * handle, void * key)
{
	binomial_tree_node_t * curr;
	binomial_tree_node_t * root;
	binomial_tree_node_t * enode;
	binomial_tree_node_t * rchild;
	binomial_tree_node_t * parent;
	map_node_t * cval;
	map_remove_helper_data_t map_remove_helper_data;
	int rc, exp;

map_remove_start:
	curr = binomial_tree_get_root(handle->tree);
	while (1)
	{	/* step 1: find the node to be deleted */
		cval = (map_node_t*)binomial_tree_node_get_value(curr);
		if (cval == NULL)
		{
			binomial_tree_node_release(curr);
			return;
		}
		if (cval->flag)
		{
			curr = binomial_tree_node_get_parent(curr);
			if (curr == NULL)
				goto map_remove_start;
			continue;
		}
		rc = handle->cmp_func(key, cval->key);
		if (rc < 0)
		{
			curr = binomial_tree_node_get_left(curr);
		}
		else if (rc > 0)
		{
			curr = binomial_tree_node_get_right(curr);
		}
		else
		{
			exp = 0;
			/* found the node to delete.. */
			if (compare_and_exchange_ptr(&exp, &(cval->flag), (void*)1))
			{
				binomial_tree_node_release(curr);
				goto map_remove_start;
			}
			break;
		}
	}
	/* step 2: re-insert anything on its left-hand side */
	binomial_tree_node_register(curr, 2);
	root = binomial_tree_node_get_left(curr);
	map_remove_helper_data.dnode_right = binomial_tree_node_get_right(curr);
	map_remove_helper_data.handle = handle;
	binomial_tree_node_foreach(root, map_remove_helper, &map_remove_helper_data);
	exp = 1;
	assert(compare_and_exchange_ptr(&exp, &(cval->flag), (void*)2) == 0);
	
	/* step 3 */
	rchild = binomial_tree_node_get_right(curr);
	enode = curr;
	assert(binomial_tree_node_set_right(curr, rchild, NULL) == 0);
	if ((parent = binomial_tree_node_get_parent(curr)) == NULL)
	{	/* curr is the root node */
		assert(binomial_tree_set_root(handle->tree, enode, curr) == 0);
		binomial_tree_node_free_deep(curr);
	}
	else
	{
		if (binomial_tree_node_get_left(parent) == curr)
		{
			assert(binomial_tree_node_set_left(parent, curr, rchild) == 0);
		}
		else if (binomial_tree_node_get_right(parent) == curr)
		{
			assert(binomial_tree_node_set_right(parent, curr, rchild) == 0);
		}
		else assert(0); /* my parent has forgotten me.. */
	}
	binomial_tree_node_free_deep(curr);
}

static map_node_t * map_node_new(void * key, void * val)
{
	map_node_t * retval = (map_node_t*)calloc(1, sizeof(map_node_t));
	
	retval->key = key;
	retval->val = val;

	return retval;
}

static void map_node_free(map_node_t * handle)
{
	free(handle);
}

