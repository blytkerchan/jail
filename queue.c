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
/* This is basically an implementation of Micheal and Scott's lock-free queue
 * algorithm. 
 * Note that no measures were taken to get rid of the ABA problem: the measure
 * proposed in the article (Siple, Fast and Practical Non-Blocking and Blocking
 * Concurrent Queue Algorithms) was not taken into account, as no double-word
 * compare-and-exchange function is available at the moment and the 
 * in-structure alignment needed for such a function is considered not to be
 * portable.
 */
#include <stdlib.h>
#include <compare_and_exchange.h>
#include "queue.h"

queue_t * new_queue(void)
{
	queue_t * retval;
	queue_node_t * dummy;
	
	retval = (queue_t*)malloc(sizeof(queue_t));
	dummy = (queue_node_t*)malloc(sizeof(queue_node_t));
	dummy->data = NULL;
	dummy->next = NULL;
	retval->head = dummy;
	retval->tail = dummy;

	return retval;
}

void free_queue(queue_t * queue)
{
	while (queue_deq(queue) != NULL);
	free(queue);
}

void queue_enq(queue_t * queue, void * data)
{
	queue_node_t * n_node;
	queue_node_t * old_tail = NULL;
	queue_node_t * old_next = NULL;
	
	n_node = (queue_node_t *)malloc(sizeof(queue_node_t));
	n_node->data = data;
	n_node->next = NULL;
	
	while (1)
	{
		old_tail = queue->tail;
		old_next = old_tail->next;
		if (old_tail != queue->tail) continue;
		if (old_next != NULL)
		{
			compare_and_exchange(&old_tail, &(queue->tail), old_next);
			continue;
		}
		if (compare_and_exchange(&old_next, &(old_tail->next), n_node) == 0)
			break;
	}
	compare_and_exchange(&old_tail, &(queue->tail), n_node);
}

void * queue_deq(queue_t * queue)
{
	queue_node_t * old_head;
	queue_node_t * old_tail;
	queue_node_t * old_next;
	void * retval;
	
	while (1)
	{
		old_head = queue->head;
		old_tail = queue->tail;
		old_next = old_head->next;
		if (old_head != queue->head) continue;
		if (old_head == old_tail)
		{
			if (old_next == NULL)
				return NULL;
			compare_and_exchange(&old_tail, &(queue->tail), old_next);
			continue;
		}
		retval = old_next->data;
		if (compare_and_exchange(&old_head, &(queue->head), old_next) == 0)
			break;
	}
	free(old_head);

	return(retval);
}

