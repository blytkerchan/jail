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
#include "libmemory/smr.h"
#include "libmemory/hptr.h"
#include "thread_queue.h"
#include "arch/increment.h"
#include "arch/compare_and_exchange.h"

#define DUMMY_MAGIC 0xdeadbeef

lt_thread_queue_t * lt_thread_queue_new(void)
{
	lt_thread_queue_t * retval;

	retval = malloc(sizeof(lt_thread_queue_t));
	retval->head = malloc(sizeof(lt_thread_t));
	retval->head->flag = DUMMY_MAGIC;
	retval->head->next = NULL;
	retval->tail = retval->head;
	
	return retval;
}

void lt_thread_queue_free(lt_thread_queue_t * queue)
{
	// This should be the dummy node - we don't check 
	free(queue->head);
	free(queue);
}

/* return nonzero if the queue is empty, zero if not */
int lt_thread_queue_empty(lt_thread_queue_t * queue)
{
	/* empty means the first node is the dummy node and its next pointer 
	 * is NULL. If the next pointer of the first node is NULL, either
	 * another thread is re-queueing the dummy node or the node is the
	 * dummy node, so we report that the queue is empty in either case. */
	lt_thread_t * head_node;
	int retval;
	
	do	// get the head node
	{
		head_node = queue->head;
		hptr_register(0, head_node);
	} while (head_node != queue->head);
	retval = (head_node->next == NULL);
	hptr_free(0);

	return retval;
}

/* return the first node, or NULL if there is none. We do not release the 
 * hazardous reference we have to it. */
lt_thread_t * lt_thread_queue_first(lt_thread_queue_t * queue)
{
	lt_thread_t * head_node;
	
	do
	{
		head_node = queue->head;
		hptr_register(0, head_node);
	} while (head_node != queue->head);
	if (head_node->flag == DUMMY_MAGIC)
	{
		hptr_free(0);
		head_node = NULL;
	}

	return head_node;
}

lt_thread_t * lt_thread_queue_deq(lt_thread_queue_t * queue)
{
	lt_thread_t * old_head;
	lt_thread_t * old_tail;
	lt_thread_t * old_next;
	
	while (1)
	{
		do
		{
			old_head = queue->head;
			hptr_register(0, old_head);
		} while (old_head != queue->head);
		do
		{
			old_tail = queue->tail;
			hptr_register(1, old_tail);
		} while (old_tail != queue->tail);
		do
		{
			old_next = old_head->next;
			hptr_register(2, old_next);
		} while (old_next != old_head->next);
		if (old_head != queue->head) 
			continue;
		if (old_head == old_tail)
		{
			if (old_next == NULL)
			{
				hptr_free(0);
				hptr_free(1);
				hptr_free(2);
				return NULL;
			}
			compare_and_exchange_ptr(&old_tail, &(queue->tail), old_next);
			continue;
		}
		if (compare_and_exchange_ptr(&old_head, &(queue->head), old_next) == 0)
			break;
	}
	if (old_head->flag == DUMMY_MAGIC)
	{
		lt_thread_queue_enq(queue, old_head);
		return lt_thread_queue_deq(queue);
	}
	hptr_free(1);
	hptr_free(2);

	return old_head;
}

void lt_thread_queue_enq(lt_thread_queue_t * queue, lt_thread_t * thread)
{
	lt_thread_t * n_node = thread;
	lt_thread_t * old_tail = NULL;
	lt_thread_t * old_next = NULL;
	
	while (1)
	{
		do
		{
			old_tail = queue->tail;
			hptr_register(0, old_tail);
		} while (old_tail != queue->tail);
		old_next = old_tail->next;
		if (old_tail != queue->tail) continue;
		if (old_next != NULL)
		{
			compare_and_exchange_ptr(&old_tail, &(queue->tail), old_next);
			continue;
		}
		if (compare_and_exchange_ptr(&old_next, &(old_tail->next), n_node) == 0)
			break;
	}
	compare_and_exchange_ptr(&old_tail, &(queue->tail), n_node);
	hptr_free(0);
}

