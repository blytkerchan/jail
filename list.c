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
#include <compare_and_exchange.h>
#include "list.h"

static void list_free_node(list_node_t * node)
{
	free(node);
}

static list_node_t * list_new_node(void * val)
{
	list_node_t * retval = (list_node_t *)malloc(sizeof(list_node_t));
	retval->mark = 0;
	retval->val = val;
	retval->next = NULL;

	return retval;
}

static list_node_t * list_find(list_t * list, void * val)
{
	int rv;
	
try_again:
	list->prev = list->head;
	while (1)
	{
		list->curr = list->prev->next;
		if (list->curr == NULL)
			return NULL;
		list->cval = list->curr->val;
		list->cmark = list->curr->mark;
		if (list->curr != list->prev->next)
			goto try_again;
		if ((rv = list->cmp_func(list->cval, val)) >= 0)
			return ((rv == 0 && !list->cmark) ? list->curr : NULL);
		list->prev = list->curr;
	}
}

static int list_insert_node(list_t * list, list_node_t * node)
{
	void * val;

	val = node->val;
	while (1)
	{
		if (list_find(list, val) != NULL)
			return -1;
		node->mark = 0;
		node->next = list->curr;
		if (compare_and_exchange(&(list->curr), &(list->prev->next), node) == 0)
			return 0;
	}
}

int list_insert(list_t * list, void * val)
{
	list_node_t * n_node = list_new_node(val);
	if (list_insert_node(list, n_node) != 0)
	{
		list_free_node(n_node);
	}
	else
		return 0;

	return -1;
}

int list_delete(list_t * list, void * val)
{
	int cmark;
	int nmark;
	
try_again:
	while (1)
	{
		if (list_find(list, val) == NULL)
			return -1;
		cmark = 0;
		nmark = 1;
		if (compare_and_exchange(&cmark, &(list->curr->mark), (void*)nmark) != 0)
			continue;
		if (compare_and_exchange(&(list->curr), &(list->prev->next), list->curr->next) == 0)
			list_free_node(list->curr);
		else
			goto try_again;

		return 0;
	}
}

void * list_search(list_t * list, void * val)
{
	list_node_t * node = list_find(list, val);
	if (node == NULL)
		return NULL;
	return node->val;
}

