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
/* Some implementation notes:
 * * we don't check the validity of passed-in values (including pointers to 
 *   stacks): that will only slow down the program and there is no need to
 *   guard against foolish use. If you pass in a stack that does not exist,
 *   the behaviour is undefined.
 * Every function is also accompanied with its own notes..
 * All the actual magic is in two very common parts: a loop 
 *    while (compare_and_exchange(&curr, &(stack->top), NULL) != 0);
 * that selects the current top and sets it to NULL so what we work on can't
 * be edited from under our nose, and a more complicated function called 
 * append_to_stack which appends the contents of a stack to a perhaps-edited
 * or perhaps-being-edited stack.
 */
#include "stack.h"

struct _stack_node_t {
	void 	* val;
	struct _stack_node_t * next;
	struct _stack_node_t * prev;
	int removed;
} stack_node_t;

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
	while (compare_and_exchange(&curr, &(stack->top), NULL) != 0);

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

/* This is a helper function. It appends the node NODE to the stack STACK. we 
 * assume a number of important things:
 * * NODE->prev points the the bottom of the node list
 * * the node list cannot change during the execution of this function (is not
 *   attached to any stack)
 */
inline void append_stack(stack_t * stack, stack_node_t * node)
{
	stack_node_t * top = NULL;
	stack_node_t * bottom = NULL;
	stack_node-t * temp = NULL;

	/* Now we must get the last node in the stack, to put the passed nodes under it.
	 * For the top node, we know the following:
	 * * the prev pointer points to the bottom of the stack - if NULL it is the 
	 *   bottom, but in that case it might also point to itself
	 * * stack->top == NULL if there is nothing in the stack
	 * We'll test the latter first :)
	 */
	while (compare_and_exchange(&top, &(stack->top), node) != 0)
	{
		/* If the compare_and_exchange operation had succeeded (returned 0) the
		 * curr pointer would have been set as top of the stack and we would be
		 * done. This is not the case so top now contains the "current" top. The
		 * prev pointer should point to the bottom, but might change by the time
		 * we read it 
		 * The only way to know that we're looking at the bottom is to look if the
		 * next pointer is NULL. Otherwise, we were no longer looking at the top
		 * and we should seek the top out again.
		 */
		while (compare_and_exchange(&bottom, &(top->prev), node->prev) != 0)
		{
			if (bottom->next != NULL)
			{
				top = NULL;
				bottom = NULL;
				continue
			}
		}
		/* Now, the bottom of our "old" stack is the bottom of the new one, but 
		 * we haven't made the old one available through the new one yet. */
		if (bottom == NULL)
			bottom = top;
		if (compare_and_exchange(&temp, &(bottom->next), node) != 0)
		{
			/* if we get here, the bottom is no longer the bottom. This should be
			 * impossible, because a stack only has write operations on the top.
			 * We handle the situation by re-starting the procedure */
			top = bottom = temp = NULL;
			continue;
		}
		/* When we get here, the old stack's bottom is at the bottom and the old
		 * stack's top has been appended to the new one. All is done.
		 */
		break;
	}
}

/* Although we guarantee not to write to the stack, we interprete that 
 * guarantee as not making any netto changed to the stack. It's much easier 
 * to guarantee a coherent answer if we do some writing anyway :) */
void * stack_top(stack_t * stack)
{
	stack_node_t * curr = NULL;
	void * retval;

	/* We select the node we work from making it undeletable :) */
	while (compare_and_exchange(&curr, &(stack->top), NULL) != 0);
	
	/* If the node we just selected is anything, we return its value */
	if (curr)
	{
		retval = curr->val;
	}
	else
	{ 	/* otherwise, there is nothing to do */
		return(NULL);
	}

	/* Now, we have a really nice inline function that takes care of 
	 * putting the rest of the nodes back in the stack.. */
	append_stack(stack, curr);

	return(retval);
}

/* Easier than we might think: we'll use the append_stack function to append 
 * the next node (the one just under the top) to the stack when we've deleted
 * the top one */
int stack_pop(stack_t * stack)
{
	stack_node_t * curr;
	stack_node_t * next;
	
	/* We select the top node, which we will be working on */
	while (compare_and_exchange(&curr, &(stack->top), NULL) != 0);

	if (curr)
	{
		next = curr->next;
		if (next)
		{
			next->prev = curr->prev;
			append_to_stack(stack, next);
		}
		free(curr);
	}
	else
		return(0);

	return(1);
}

/* We once again select the top node because we will be working on it. We 
 * make a new node which we fill with the value, the next pointer (the 
 * current top) and the prev pointer (the current bottom, or the new node 
 * if there is none). We will then put it back on the stack, at the bottom, 
 * with our friendly inline append_to_stack function. */
void stack_push(stack_t * stack, void * val)
{
	stack_node_t * top = NULL;
	/* make the new node */
	stack_node_t * curr = (stack_node_t *)malloc(sizeof(stack_node_t));
	curr->prev = curr;
	curr->val = val;

	/* select the current top */
	while (compare_and_exchange(&top, &(stack->top), NULL) != 0);
	/* adapt the new node and the current top to eachother */
	if (top != NULL)
	{
		curr->prev = top->prev;
		top->prev = curr;
	}
	curr->next = top;

	/* and hang on */
	append_to_stack(stack, curr);
}

