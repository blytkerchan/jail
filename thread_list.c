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
#include "thread_list.h"
#define DUMMY_MAGIC 0xdeadbeef;

typedef struct _lt_thread_list_state_t
{
	lt_thread_t * prev;
	lt_thread_t * curr;
	lt_thread_t * next;
	int pmark;
	int cmarl;
} lt_thread_list_state_t;

static lt_thread_t * lt_thread_list_find(lt_thread_list_state_t * state, lt_thread_list_t * list, lt_thread_t * thread)
{
	int rv;
	
try_again:
	/* the hazardous reference here is a slight modification of MM Micheal's algorithm: 
	 * in the original algorithm, no hazard pointer is used to hold prev if prev is
	 * the head of the list. I do this mostly for consistency: hptr2 contains prev, hptr0
	 * contains next and hptr1 contains curr */
	do
	{
		state->prev = list->head;
		hptr_register(2, state->prev);
	} while (state->prev != list->head);
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
		if ((state->prev->mark != 0) || (state->prev->next != state->curr))
			goto try_again;
		if (state->cmark == 0)
		{
			rv = thread - state->curr;
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

lt_thread_list_t * lt_thread_list_new(void)
{
	lt_thread_list_t *  retval;
	
	retval = malloc(sizeof(lt_thread_list_t));
	retval->head = malloc(sizeof(lt_thread_t));
	retval->head->next = NULL;
	retval->head->flag = DUMMY_MAGIC;
	
	return retval;
}

void lt_thread_list_free(lt_thread_list_t * list)
{
	free(list->head);
	free(list);
}

int lt_thread_list_remove(lt_thread_list_t * list, lt_thread_t * entry)
{
	int retval;
	int cmark;
	int nmark;
	list_state_t state;
	state.prev = NULL;
	state.curr = NULL;
	state.cval = NULL;
	state.cmark = 0;

	while (1)
	{
		if (list_find(&state, list, val) == NULL)
		{
			retval = -1;
			break;
		}
		cmark = 0;
		nmark = 1;
		if (compare_and_exchange_ptr(&cmark, &(state.curr->mark), (void*)nmark) != 0)
			continue;
		if (compare_and_exchange_ptr(&(state.curr), &(state.prev->next), state.next) == 0)
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

/* the use of hazard pointers is actually overkill here, but let's be consistent.. */
int lt_thread_list_empty(lt_thread_list_t * list)
{
	lt_thread_t * curr;
	int retval;
	
	do
	{
		curr = list->head;
		hptr_register(0, curr);
	} while (curr != list->head);
	retval = curr->next == NULL;
	hptr_free(0);
	
	return retval;
}

void lt_thread_list_move(lt_thread_list_t * to, lt_thread_list_t * from)
{
	lt_thread_t * head;
	lt_thread_t * curr;
	lt_thread_t * next;
	
start_again:
	do	/* unneeded hazardous reference used for consistency */
	{
		head = from->head;
		hptr_register(0, head);
	} while (head != from->head);

	while (head->next != NULL)
	{
		do
		{
			curr = head->next;
			hptr_register(1, curr);
		} while (curr != head->next);
		do
		{
			next = curr->next;
			hptr_register(2, next);
		} while (next != curr->next);
		if (compare_and_exchange_ptr(&curr, &(head->next), next) != 0)
			goto start_again;
		lt_thread_list_insert(to, curr);
	}
}

void lt_thread_list_for_each(lt_thread_list_t * list, void (*)(lt_thread_t * thread));
void lt_thread_list_insert(lt_thread_list_t * list, lt_thread_t * thread);
lt_thread_t * lt_thread_list_find(lt_thread_list_t * list, lt_rwlock_t * thread);

