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
#include "arch/include/compare_and_exchange.h"
#include <libmemory/smr.h>
#include <libmemory/hptr.h>
#include "list_node.h"

void list_node_free(list_node_t * node)
{
	free(node);
}

list_node_t * list_node_new(void * val)
{
	list_node_t * retval = (list_node_t *)malloc(sizeof(list_node_t));
	retval->mark = 0;
	retval->val = val;
	retval->next = NULL;

	return retval;
}

/* a hazardous reference to the return value is held by hptr 1; its prev in 
 * hptr 0 and its next in hptr 2 */
list_node_t * list_node_find(list_state_t * state, list_node_t * head, libcontain_cmp_func_t cmp_func, void * val)
{
	int rv;
	
try_again:
	/* the hazardous reference here is a slight modification of 
	 * MM Micheal's algorithm: in the original algorithm, no hazard 
	 * pointer is used to hold prev if prev is the head of the list. I 
	 * do this mostly for consistency: hptr2 contains prev, hptr0
	 * contains next and hptr1 contains curr */
	do
	{
		state->prev = head;
		hptr_register(2, state->prev);
	} while (state->prev != head);
	state->pmark = state->prev->mark;
	state->curr = state->prev->next;
	hptr_register(1, state->curr);
	if ((state->pmark != 0) || (state->prev->mark != 0) || (state->curr != state->prev->next))
		goto try_again;
	while (1)
	{
		if (state->curr == NULL)
			return NULL;
		state->next = state->curr->next;
		state->cmark = state->curr->mark;
		hptr_register(0, state->next);
		if ((state->next != state->curr->next) || (state->cmark != state->curr->mark))
			goto try_again;
		state->cval = state->curr->val;
		if ((state->prev->mark != 0) || (state->prev->next != state->curr))
			goto try_again;
		if (state->cmark == 0)
		{
			rv = cmp_func(state->cval, val);
			if (rv > 0)
				return NULL;
			if (rv == 0)
				return state->curr;
			state->prev = state->curr;
			hptr_register(2, state->prev);
		}
		else
		{
			if (compare_and_exchange_ptr(&(state->curr), &(state->prev->next), state->next) == 0)
				free(state->curr);
			else goto try_again;
		}
		state->curr = state->next;
		hptr_register(1, state->curr);
	}
}

int list_node_insert(list_node_t * head, libcontain_cmp_func_t cmp_func, list_node_t * node)
{
	void * val;
	int retval;
	list_state_t * state = (list_state_t*)alloca(sizeof(list_state_t));
	state->prev = NULL;
	state->curr = NULL;
	state->cval = NULL;
	state->cmark = 0;

	val = node->val;
	while (1)
	{
		if (list_node_find(state, head, cmp_func, val) != NULL)
		{
			retval = -1;
			break;
		}
		node->mark = 0;
		node->next = state->curr;
		if (compare_and_exchange_ptr(&(state->curr), &(state->prev->next), node) == 0)
		{
			retval = 0;
			break;
		}
	}
	hptr_free(0);
	hptr_free(1);
	hptr_free(2);

	return retval;
}

void list_node_foreach(list_node_t * head, libcontain_foreach_func_t helper, void * data)
{
	list_node_t * curr;
	list_node_t * next;
	
	do
	{
		curr = head;
		hptr_register(0, curr);
	} while (curr != head);
	while (curr != NULL)
	{
		helper(curr->val, data);
		do
		{
			next = curr->next;
			hptr_register(1, next);
		} while (next != curr->next);
		curr = next;
		hptr_register(0, curr);
	}
	hptr_free(0);
	hptr_free(1);
}

