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
#include <libmemory/smr.h>
#include <libmemory/hptr.h>
#include "arch/include/compare_and_exchange.h"
#include "binomial_tree.h"
#include "binary.h"

binomial_tree_t * binomial_tree_new(void)
{
	binomial_tree_t * retval = (binomial_tree_t*)calloc(1, sizeof(binomial_tree_t));

	return retval;
}

void binomial_tree_free(binomial_tree_t * tree)
{
	if (tree->trunk != NULL)
		binomial_tree_node_free_deep(tree->trunk);
	free(tree);
}

binomial_tree_node_t * binomial_tree_get_root(binomial_tree_t * handle)
{
	binomial_tree_node_t * top;
	binomial_tree_node_t * curr;
	
	do
	{
		while ((top = handle->trunk) == NULL)
		{	/* create a trunk if need be */
			curr = binomial_tree_node_new(NULL);
			if (compare_and_exchange(&top, &(handle->trunk), curr) != 0)
			{
				binomial_tree_node_free(curr);
			}
		}
		hptr_register(0, top);
	} while (top != handle->trunk);

	return top;
}

binomial_tree_node_t * binomial_tree_node_new(binomial_tree_node_t * parent)
{
	binomial_tree_node_t * retval = (binomial_tree_node_t*)calloc(1, sizeof(binomial_tree_node_t));
	retval->parent = parent;

	return retval;
}

void binomial_tree_node_free(binomial_tree_node_t * handle)
{
	free(handle);
}

/* set the root of the tree iff it is currently EXP.
 * return 0 if successful, nonzero if not 
 * --
 * failing to set the parent 
 */
int binomial_tree_set_root(binomial_tree_t * tree, binomial_tree_node_t * exp, binomial_tree_node_t * val)
{
	binomial_tree_node_t * o_parent = val->parent;
	
	if (compare_and_exchange(&exp, &(tree->trunk), val) == 0)
	{
		compare_and_exchange(&o_parent, &(val->parent), NULL);
		return 0;
	}
	return 1;
}

/* delete a node and its children */
void binomial_tree_node_free_deep(binomial_tree_node_t * handle)
{
	if (handle->left != NULL)
		binomial_tree_node_free_deep(handle->left);
	if (handle->right != NULL)
		binomial_tree_node_free_deep(handle->right);
	binomial_tree_node_free(handle);
}

/* get the left-hand child of a node, release the parent node */
binomial_tree_node_t * binomial_tree_node_get_left(binomial_tree_node_t * node)
{
	binomial_tree_node_t * o_node;
	binomial_tree_node_t * n_node;
	
	do
	{
		o_node = node->left;
		if (o_node != NULL)
			hptr_register(0, o_node);
	} while (o_node != node->left);
	if (o_node != NULL)
		return o_node;
	n_node = binomial_tree_node_new(node);
	if (binomial_tree_node_set_left(node, NULL, n_node))
		binomial_tree_node_free(n_node);
	return binomial_tree_node_get_left(node);
}

/* get the right-hand child of a node, release the parent node */
binomial_tree_node_t * binomial_tree_node_get_right(binomial_tree_node_t * node)
{
	binomial_tree_node_t * o_node;
	binomial_tree_node_t * n_node;
	
	do
	{
		o_node = node->right;
		hptr_register(0, o_node);
	} while (o_node != node->right);
	if (o_node != NULL)
		return o_node;
	n_node = binomial_tree_node_new(node);
	if (binomial_tree_node_set_right(node, NULL, n_node))
		binomial_tree_node_free(n_node);
	return binomial_tree_node_get_right(node);
}

int binomial_tree_node_set_left(binomial_tree_node_t * node, binomial_tree_node_t * exp, binomial_tree_node_t * nval)
{
	binomial_tree_node_t * o_parent = nval->parent;
	
	if (compare_and_exchange(&exp, &(node->left), nval) == 0)
	{
		compare_and_exchange(&o_parent, &(nval->parent), node);
		return 0;
	}
	
	return 1;
}

int binomial_tree_node_set_right(binomial_tree_node_t * node, binomial_tree_node_t * exp, binomial_tree_node_t * nval)
{
	binomial_tree_node_t * o_parent = nval->parent;
	
	if (compare_and_exchange(&exp, &(node->right), nval) == 0)
	{
		compare_and_exchange(&o_parent, &(nval->parent), node);
		return 0;
	}
	
	return 1;
}

binomial_tree_node_t * binomial_tree_node_get_parent(binomial_tree_node_t * node)
{
	binomial_tree_node_t * retval;

	do
	{
		retval = node->parent;
		hptr_register(0, retval);
	} while (retval != node->parent);
	
	return retval;
}

void binomial_tree_node_register(binomial_tree_node_t * node, int reg) 
{
	hptr_register(reg, node);
}

void binomial_tree_node_release(binomial_tree_node_t * node)
{
	unsigned int i;
	
	for (i = 0; i < LIBCONTAIN_MIN_HPTRS; i++)
	{
		if (hptr_get(i) == node)
			hptr_free(i);
	}
}

void * binomial_tree_node_get_value(binomial_tree_node_t * node)
{
	return node->val;
}

int binomial_tree_node_set_value(binomial_tree_node_t * node, void * curr, void * val)
{
	return compare_and_exchange(&curr, &(node->val), val);
}

static binomial_tree_node_t * binomial_tree_node_select(binomial_tree_node_t * root, unsigned int level, unsigned int nodeind)
{
	unsigned int h, n, K, F, r;

	h = level + 1;
	n = pow2(level) + nodeind - 1;
binomial_tree_node_select_start:
	K = pow2(h) - 1;
	F = pow2(h - 2);
	r = K - n;

	if (n == 0)
		return root;
	if (r > F)
	{
		root = binomial_tree_node_get_left(root);
		h--;
		n -= F;
		goto binomial_tree_node_select_start;
	}
	else
	{
		root = binomial_tree_node_get_right(root);
		h--;
		n -= (2 * F);
		goto binomial_tree_node_select_start;
	}
}

/* The algorithm implemented by this function minimalizes the number of 
 * hazardous references we have to the nodes while going by each node to
 * perform the action described by FUNC. 
 * Now, what we know about the binomial tree:
 * * the maximal number of nodes per level is 2^level, where level starts
 *   at 0.
 * * The root node has a permanent hazardous reference, which means we can 
 *   always go down to the node we want from the root.
 * This doesn't make for the most time-efficient traversal algorithm ever 
 * known, but it does make for an algorithm that only needs two hazardous
 * references. A much more time-efficient algorithm would be to traverse
 * the tree recursively. However, that would mean using either an infinite
 * amount of hazardous references (as many hazardous references as the
 * number of nodes in the path from the root to the furthest leaf) or 
 * blocking the tree while traversing it. We don't have an infinite number
 * of hazard pointers (we have three) and we don't want to lock the tree.
 * The select algorithm is a non-recursive adaption of the one found in
 * the file ``how_heaps_work''. */ 
void binomial_tree_node_foreach(binomial_tree_node_t * root, libcontain_foreach_func_t func, void * data)
{
	unsigned int level = 0;
	unsigned int nodeind = 0;
	int found_node = 0;
	binomial_tree_node_t * node;
	void * val;
	
	hptr_register(1, root);

	if (root->val)
		func(root->val, data);
	level = 1;
	while (1)
	{
		found_node = 0;
		for (nodeind = 0; nodeind < pow2(level); nodeind++)
		{
			node = binomial_tree_node_select(root, level, nodeind);
			if ((val = node->val) != NULL)
			{
				found_node = 1;
				func(val, data);
			}
		}
		if (found_node)
		{
			level++;
		}
		else
			break;
	}
	hptr_register(0, root);
	hptr_free(1);
}

void binomial_tree_foreach(binomial_tree_t * handle, libcontain_foreach_func_t func, void * data)
{
	binomial_tree_node_t * root;

	root = binomial_tree_get_root(handle);
	binomial_tree_node_foreach(root, func, data);
	binomial_tree_node_release(root);
}

