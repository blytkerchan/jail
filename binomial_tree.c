
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
#include "binomial_tree.h"

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
	
	while ((top = handle->trunk) == NULL)
	{
		curr = binomial_tree_node_new(NULL);
		if (compare_and_exchange(&top, &(handle->trunk), curr) != 0)
		{
			binomial_tree_node_free(curr);
		}
	}

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
	
	if ((o_node = node->left) != NULL)
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
	
	if ((o_node = node->right) != NULL)
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
	return node->parent;
}

void binomial_tree_node_register(binomial_tree_node_t * node, int reg) 
{ /* no-op */ }

void binomial_tree_node_release(binomial_tree_node_t * node)
{ /* no-op */ }

void * binomial_tree_node_get_value(binomial_tree_node_t * node)
{
	return node->val;
}

int binomial_tree_node_set_value(binomial_tree_node_t * node, void * curr, void * val)
{
	return compare_and_exchange(&curr, &(node->val), val);
}

void binomial_tree_node_foreach(binomial_tree_node_t * root, binomial_tree_node_foreach_func_t func, void * data)
{
	if (root->left)
		binomial_tree_node_foreach(root->left, func, data);
	if (root->right)
		binomial_tree_node_foreach(root->right, func, data);
	if (root->val)
		func(root->val, data);
}

void binomial_tree_foreach(binomial_tree_t * handle, binomial_tree_node_foreach_func_t func, void * data)
{
	binomial_tree_node_foreach(binomial_tree_get_root(handle), func, data);
}

