#include "critical_section.h"
#include <stdlib.h>
#include <assert.h>
#include "libmemory/hptr.h"
#include "arch/include/compare_and_exchange.h"

critical_section_t * critical_section_new(void)
{
	critical_section_t * retval;

	retval = calloc(1, sizeof(critical_section_t));
	assert(retval != NULL);
	retval->head = retval->tail = calloc(1, sizeof(lt_thread_t));
	assert(retval->head != NULL);

	return retval;
}

void critical_section_free(critical_section_t * critical_section)
{
	free(critical_section);
}

void critical_section_enter(critical_section_t * critical_section)
{
	lt_thread_t * old_next;
	lt_thread_t * old_tail;
	lt_thread_t * n_node = lt_thread_self();
	lt_thread_t * head_node;

	/* enqueue the current thread in the critical section's queue */
	while (1)
	{
		do
		{
			old_tail = critical_section->tail;
			hptr_register(0, old_tail);
		} while (old_tail != critical_section->tail);
		old_next = old_tail->next;
		if (old_tail != critical_section->tail)
			continue;
		if (old_next != NULL)
		{
			compare_and_exchange_ptr(&old_tail, &(critical_section->tail), old_next);
			continue;
		}
		
		if (compare_and_exchange_ptr(&old_next, &(old_tail->next), n_node) == 0)
			break;
	}
	compare_and_exchange_ptr(&old_tail, &(critical_section->tail), n_node);
	hptr_free(0);

	n_node = lt_thread_self();
#if CRITICAL_SECTION_USE_SEM
	while (sem_try_wait(&(n_node->priv_sem), 0, 0) != EAGAIN);
#endif
	/* wait for the first enqueued thread to be us */
	do
	{
		hptr_free(0);
		do
		{
			head_node = critical_section->head->next;
			hptr_register(0, head_node);
		} while (head_node != critical_section->head->next);
#if CRITICAL_SECTION_USE_SEM
		if (lt_thread_eq(head_node, n_node) != 0)
		{
			sem_wait(n_node->priv_sem);
			continue;
		}
#endif
	} while (lt_thread_eq(head_node, n_node) != 0);
	hptr_free(0);
}

void critical_section_leave(critical_section_t * critical_section)
{
	lt_thread_t * head_node;
	lt_thread_t * new_head;
	lt_thread_t * exp;
	lt_thread_t * old_tail;
	
	/* dequeue us */
	head_node = critical_section->head->next;
	assert(lt_thread_eq(head_node, lt_thread_self()) == 0);
	do
	{
		new_head = head_node->next;
		exp = head_node;
		do
		{
			old_tail = critical_section->tail;
			hptr_register(0, old_tail);
		} while (old_tail != critical_section->tail);
		assert(compare_and_exchange_ptr(&exp, &(critical_section->head->next), new_head) == 0);
		exp = head_node;
		compare_and_exchange_ptr(&exp, &(critical_section->tail), critical_section->head);
	} while (compare_and_exchange_ptr(&new_head, &(head_node->next), NULL) != 0);
#if CRITICAL_SECTION_USE_SEM
	sem_post(&(new_head->priv_sem));
#endif
}

