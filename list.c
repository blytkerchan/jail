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
#include "arch/compare_and_exchange.h"
#include "libmemory/smr.h"
#include "libmemory/hptr.h"
#include "list.h"
#include "list_node.h"

int list_insert(list_t * list, void * val)
{
	list_node_t * n_node = list_node_new(val);
	if (list_node_insert(list->head, list->cmp_func, n_node) != 0)
	{
		list_node_free(n_node);
	}
	else
		return 0;

	return -1;
}

int list_delete(list_t * list, void * val)
{
	int retval;
	int cmark;
	int nmark;
	list_state_t * state = (list_state_t*)alloca(sizeof(list_state_t));
	state->prev = NULL;
	state->curr = NULL;
	state->cval = NULL;
	state->cmark = 0;

	while (1)
	{
		if (list_node_find(state, list->head, list->cmp_func, val) == NULL)
		{
			retval = -1;
			break;
		}
		cmark = 0;
		nmark = 1;
		if (compare_and_exchange_ptr(&cmark, &(state->curr->mark), (void*)nmark) != 0)
			continue;
		if (compare_and_exchange_ptr(&(state->curr), &(state->prev->next), state->next) == 0)
		{
			list_node_free(state->curr);
			retval = 0;
			break;
		}
	}

	hptr_free(0);
	hptr_free(1);
	hptr_free(2);
	
	return retval;
}

void * list_search(list_t * list, void * val)
{
	list_node_t * node;
	list_state_t * state = (list_state_t*)alloca(sizeof(list_state_t));
	state->prev = NULL;
	state->curr = NULL;
	state->cval = NULL;
	state->cmark = 0;
	
	node = list_node_find(state, list->head, list->cmp_func, val);

	hptr_free(0);
	hptr_free(1);
	hptr_free(2);
	
	if (node == NULL)
		return NULL;
	return state->cval;
}

list_t * list_new(libcontain_cmp_func_t cmp_func)
{
	list_t * retval = (list_t*)malloc(sizeof(list_t));
	retval->cmp_func = cmp_func;
	retval->head = list_node_new(NULL);

	return retval;
}

void list_free(list_t * list)
{
	list_node_t * curr;
	list_node_t * next;

	curr = list->head;
	while (curr)
	{
		next = curr->next;
		free(curr);
		curr = next;
	}
	free(list);
}

void list_foreach(list_t * list, libcontain_foreach_func_t helper, void * data)
{
	list_node_foreach(list->head, helper, data);
}

