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
#include <stdlib.h>
#include "arch/include/compare_and_exchange.h"
#include "arch/include/increment.h"
#include "arch/include/decrement.h"
#include "heap.h"
#include "binomial_tree.h"
#include "binary.h"
#include "thread.h"

/* this should tell the system that the rest of the time-slice is not interesting for us - it may be handed over to someone else.. */
#define thread_interrupt() sleep(0)
/* get a non-exclusive lock on the heap, wait for exclusive locks to go away */
#define SOFT_LOCK(handle)																	\
	atomic_increment((uint32_t*)&(handle->workers));								\
	while (handle->flag)																		\
	{																								\
		atomic_decrement((uint32_t*)&(handle->workers));							\
		thread_interrupt();																	\
		atomic_increment((uint32_t*)&(handle->workers));											\
	}
/* release non-exclusive lock on the heap */
#define SOFT_UNLOCK(handle) atomic_decrement((uint32_t*)&(handle->workers))
/* get an exclusive lock on the heap, wait for non-exclusive locks to go away */
#define HARD_LOCK(handle)																	\
	atomic_increment((uint32_t*)&(handle->flag));									\
	atomic_increment((uint32_t*)&(handle->workers));								\
	while ((handle->workers != 1) || (handle->flag != 1))							\
	{																								\
		atomic_decrement((uint32_t*)&(handle->flag));								\
		atomic_decrement((uint32_t*)&(handle->workers));							\
		thread_interrupt();																	\
		atomic_increment((uint32_t*)&(handle->workers));							\
		atomic_increment((uint32_t*)&(handle->flag));								\
	}
/* release an exclusive lock on the heap, leave a non-exclusive one */
#define HARD_UNLOCK(handle)																\
	do																								\
	{																								\
		atomic_decrement((uint32_t*)&(handle->flag));								\
	}																								\
	while (0)

static heap_node_t * heap_node_new(void)
{
	heap_node_t * retval = calloc(1, sizeof(heap_node_t));

	return retval;
}

static void heap_node_free(heap_node_t * node)
{
	free(node);
}

heap_t * heap_new(libcontain_cmp_func_t heap_val_cmp_func)
{
	heap_t * retval = (heap_t*)calloc(1, sizeof(heap_t));
	retval->tree = binomial_tree_new();
	retval->heap_val_cmp_func = heap_val_cmp_func;

	return retval;
}

static void heap_free_helper(const void * dat1, void * dat2)
{
	free((void*)dat1);
}

void heap_free(heap_t * handle)
{
	
	binomial_tree_foreach(handle->tree, heap_free_helper, NULL);
	binomial_tree_free(handle->tree);
	free(handle);
}

static binomial_tree_node_t * heap_select_node(binomial_tree_node_t * root, size_t h, size_t n)
{
	heap_node_t * node;
	size_t k, r;
	int exp;
	
	if (n == 0)
	{
		if (root->val == NULL)
		{
			node = heap_node_new();
			node->flag = 1;
			if (binomial_tree_node_set_value(root, NULL, node) != 0)
				heap_node_free(node);
			else
				return root;
		}
		exp = 0;
		if (compare_and_exchange(&exp, &(((heap_node_t*)(root->val))->flag), (void*)1) != 0)
			return NULL;
		return root;
	}
		
	k = pow2(h) - 1;
	r = k - n;
	if (r >= pow2(h - 1))
		return heap_select_node(binomial_tree_node_get_left(root), h - 1, n - pow2(h - 2));
	else
		return heap_select_node(binomial_tree_node_get_right(root), h - 1, n - pow2(h - 1));
}

void heap_add(heap_t * handle, void * val)
{
	size_t o_n;
	size_t n;
	binomial_tree_node_t * node = NULL;
	binomial_tree_node_t * parent;
	heap_node_t * heap_node;
	heap_node_t * heap_parent_node;
	int exp, rc;

	SOFT_LOCK(handle);
	do
	{
		o_n = handle->N;
		n = o_n + 1;
	} while (compare_and_exchange(&o_n, &(handle->N), (void*)n));
	/* these two should never fail.. */
	node = heap_select_node(binomial_tree_get_root(handle->tree), lg(n) + 1, n - 1);
	heap_node = binomial_tree_node_get_value(node);
	heap_node->val = val;
	while (1)
	{
		do {
			exp = 0;
			binomial_tree_node_register(node, 1);
			if ((parent = binomial_tree_node_get_parent(node)) == NULL)
			{	/* root node */
				heap_node->flag = 0;
				SOFT_UNLOCK(handle);
				return;
			}
			if ((heap_parent_node = (heap_node_t*)binomial_tree_node_get_value(parent)) == NULL)
				continue;
		} while (compare_and_exchange(&exp, &(((heap_node_t*)(parent->val))->flag), (void*)1) != 0);
		rc = handle->heap_val_cmp_func(heap_node->val, heap_parent_node->val);
		if (rc <= 0)
		{
			heap_parent_node->flag = 0;
			heap_node->flag = 0;
			SOFT_UNLOCK(handle);
			return;
		}
		/* these two can't fail */
		binomial_tree_node_set_value(parent, heap_parent_node, heap_node);
		binomial_tree_node_set_value(node, heap_node, heap_parent_node);
		heap_parent_node->flag = 0;
		binomial_tree_node_release(node);
		node = parent;
	}
	/* unreachable, but a reminder to the developer */
	SOFT_UNLOCK(handle);
}

void * heap_top(heap_t * handle)
{
	binomial_tree_node_t * node;
	heap_node_t * heap_node;
	void * retval;

	SOFT_LOCK(handle);
	node = binomial_tree_get_root(handle->tree);
	heap_node = (heap_node_t*)binomial_tree_node_get_value(node);
	if (heap_node == NULL)
	{
		SOFT_UNLOCK(handle);
		return NULL;
	}
	retval = heap_node->val;
	binomial_tree_node_release(node);

	SOFT_UNLOCK(handle);
	return retval;
}

static void heap_fix(heap_t * handle, binomial_tree_node_t * root)
{
	binomial_tree_node_t * left;
	binomial_tree_node_t * right;
	binomial_tree_node_t * largest_child;
	heap_node_t * root_node;
	heap_node_t * left_node;
	heap_node_t * right_node;
	heap_node_t * largest_child_node;
	int exp;

	if ((root_node = binomial_tree_node_get_value(root)) == NULL)
		return;
	left = binomial_tree_node_get_left(root);
	right = binomial_tree_node_get_right(root);
	/* This is a left-complete tree. If there is no left child, we're a leaf. */
	do
	{
		exp = 0;
		if ((left_node = binomial_tree_node_get_value(left)) == NULL)
		{
			root_node->flag = 0;
			return;
		}
		if (left_node->val == NULL)
		{
			root_node->flag = 0;
			return;
		}
	} while (compare_and_exchange(&exp, &(left_node->flag), (void*)1) != 0);
	do
	{
		exp = 0;
		if ((right_node = binomial_tree_node_get_value(right)) == NULL)
			break;
	} while (compare_and_exchange(&exp, &(right_node->flag), (void*)1));

	largest_child = left;
	largest_child_node = left_node;
	if (right_node)
	{
		if (right_node->val != NULL)
		{
			if (handle->heap_val_cmp_func(right_node->val, left_node->val) > 0)
			{
				largest_child = right;
				largest_child_node = right_node;
			}
		}
	}
	
	if (largest_child == right)
		left_node->flag = 0;
	else if (right_node != NULL)
		right_node->flag = 0;
	if (handle->heap_val_cmp_func(largest_child_node, root_node) > 0)
	{
		binomial_tree_node_set_value(root, root_node, largest_child_node);
		binomial_tree_node_set_value(largest_child, largest_child_node, root_node);
		largest_child_node->flag = 0;
		heap_fix(handle, largest_child);
		return;
	}
	root_node->flag = 0;
	largest_child_node->flag = 0;
}

void heap_pop(heap_t * handle)
{
	binomial_tree_node_t * root;
	binomial_tree_node_t * leaf;
	heap_node_t * heap_root;
	heap_node_t * heap_leaf;
	
	HARD_LOCK(handle);
	root = binomial_tree_get_root(handle->tree);
	heap_root = binomial_tree_node_get_value(root);
	if (heap_root == NULL)
	{
		HARD_UNLOCK(handle);
		SOFT_UNLOCK(handle);
		return;
	}
	binomial_tree_node_set_value(root, heap_root, NULL);
	heap_node_free(heap_root);
	(handle->N)--;
	if (handle->N == 0)
	{
		HARD_UNLOCK(handle);
		SOFT_UNLOCK(handle);
		return;
	}
	leaf = heap_select_node(root, lg(handle->N) + 1, handle->N);
	heap_leaf = (heap_node_t*)binomial_tree_node_get_value(leaf);
	binomial_tree_node_set_value(root, NULL, heap_leaf);
	binomial_tree_node_set_value(leaf, heap_leaf, NULL);
	HARD_UNLOCK(handle);
	heap_fix(handle, root);
	SOFT_UNLOCK(handle);
}

