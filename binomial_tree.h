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
#ifndef _LIBCONTAIN_BINOMIAL_TREE_H
#define _LIBCONTAIN_BINOMIAL_TREE_H

typedef struct _binomial_tree_node_t {
	void * val;
	struct _binomial_tree_node_t * left;
	struct _binomial_tree_node_t * right;
	struct _binomial_tree_node_t * parent;
} binomial_tree_node_t;

typedef struct {
	binomial_tree_node_t * trunk;
} binomial_tree_t;

typedef void (*binomial_tree_node_foreach_func_t)(void * val, void * data);

/* create a new, empty, binomial tree */
binomial_tree_t * binomial_tree_new(void);
/* delete a binomial tree and all of its nodes */
void binomial_tree_free(binomial_tree_t * tree);
/* get the top of the tree. If the tree is new, this will create an empty node */
binomial_tree_node_t * binomial_tree_get_root(binomial_tree_t * tree);
/* set the root of the tree iff it is currently EXP. return 0 if successful, nonzero if not */
int binomial_tree_set_root(binomial_tree_t * tree, binomial_tree_node_t * exp, binomial_tree_node_t * val);
/* create a new, empty node */
binomial_tree_node_t * binomial_tree_node_new(binomial_tree_node_t * parent);
/* delete a node, but not its children */
void binomial_tree_node_free(binomial_tree_node_t * handle);
/* delete a node and its children */
void binomial_tree_node_free_deep(binomial_tree_node_t * handle);
/* get the left-hand child of a node, release the parent node */
binomial_tree_node_t * binomial_tree_node_get_left(binomial_tree_node_t * node);
/* get the right-hand child of a node, release the parent node */
binomial_tree_node_t * binomial_tree_node_get_right(binomial_tree_node_t * node);
int binomial_tree_node_set_left(binomial_tree_node_t * node, binomial_tree_node_t * exp, binomial_tree_node_t * nval);
int binomial_tree_node_set_right(binomial_tree_node_t * node, binomial_tree_node_t * exp, binomial_tree_node_t * nval);

/* get the parent of the current node; return NULL if this is the top node */
binomial_tree_node_t * binomial_tree_node_get_parent(binomial_tree_node_t * node);
/* register a node as in-use so the auto-release mechanism won't work. 
 * REG is an integer value <= 2. REG == 0 is reserve for auto-released nodes,
 * which leaves REG == 1 (used by binomial_tree_node_foreach) and REG == 2 
 * (unused). If your function uses binomial_tree_node_foreach, use REG == 2 if
 * you don't want to lose registration.
 * When the auto-release mechanism doesn't work, you have to explicitly call
 * binomial_tree_node_release to release the node. A released node is liable 
 * to be freed at some point (i.e. binomial_tree_node_free won't take effect
 * until the node has been released). */
void binomial_tree_node_register(binomial_tree_node_t * node, int reg);
/* release any reference to the node */
void binomial_tree_node_release(binomial_tree_node_t * node);
/* get the value of the node */
void * binomial_tree_node_get_value(binomial_tree_node_t * node);
/* set the value of the node iff it is the same as curr currently
 * return 0 if successful, nonzero if not. */
int binomial_tree_node_set_value(binomial_tree_node_t * node, void * curr, void * val);
/* walk the entire tree and call func on the value of each node. Pass data along to func for each call. */
void binomial_tree_node_foreach(binomial_tree_node_t * root, binomial_tree_node_foreach_func_t func, void * data);
void binomial_tree_foreach(binomial_tree_t * handle, binomial_tree_node_foreach_func_t func, void * data);
#endif
