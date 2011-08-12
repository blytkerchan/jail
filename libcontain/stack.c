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
#include "libmemory/smr.h"
#include "libmemory/hptr.h"
#include "arch/compare_and_exchange.h"
#include "stack.h"

/* This one is easy: we are the only ones working on the stack because it 
 * has not been made available to anyone else yet :) */
stack_t * new_stack(void)
{
	stack_t * retval = (stack_t*)malloc(sizeof(stack_t));
	
	if (retval)
		retval->top = NULL;

	return(retval);
}

/* This one is pretty easy as well, as we don't guarantee anything about the
 * behaviour of the stack once we've entered this function. */
void free_stack(stack_t * stack)
{
	stack_node_t * curr = NULL;
	stack_node_t * prev = NULL;

	/* we atomically put the current top in cur_top and set it to NULL. When
	 * done, we're guaranteed to have the latest top in cur_top. */
	while (compare_and_exchange_ptr(&curr, &(stack->top), NULL) != 0);

	/* now for each node in the stack, we delete it */
	while (curr)
	{
		prev = curr;
		curr = curr->next;
		free(prev);
	}

	/* and we free the descriptor itself.. */
	free(stack);
}

void * stack_top(stack_t * stack)
{
	stack_node_t * curr;
	void * retval;
	
	do
	{
		do
		{
			curr = stack->top;
			hptr_register(0, curr);
		} while (curr != stack->top);
		if (curr)
		{
			retval = curr->val;
		} else retval = NULL;
	} while (curr != stack->top);
	
	hptr_free(0);
	
	return retval;
}

int stack_pop(stack_t * stack)
{
	stack_node_t * curr;
	stack_node_t * next;
	int retval;
	
	while (1)
	{
		do
		{
			curr = stack->top;
			hptr_register(0, curr);
		} while (curr != stack->top);
		if (curr)
		{
			next = curr->next;
			hptr_register(1, next);
			if ((next != curr->next) || (curr != stack->top))
				continue;
			if (compare_and_exchange_ptr(&curr, &(stack->top), curr->next) == 0)
			{
				free(curr);
				retval = 1;
				break;
			}
		}
		else 
		{
			retval = 0;
			break;
		}
	}
	hptr_free(0);
	hptr_free(1);
	
	return retval;
}

void stack_push(stack_t * stack, void * val)
{
	stack_node_t * new_node = (stack_node_t*)malloc(sizeof(stack_node_t));
	stack_node_t * top = NULL;
	
	do
	{
		top = stack->top;
		new_node->val = val;
		new_node->next = top;
	} while (compare_and_exchange_ptr(&top, &(stack->top), new_node) != 0);
}
