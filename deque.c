#include <assert.h>
#include <stdlib.h>
#include "../libmemory/smr.h"
#include "../arch/include/compare_and_exchange.h"
#include "../arch/include/swap.h"
#include "deque.h"

typedef enum {STABLE, RPUSH, LPUSH} status_type;
typedef union deque_stat_type
{
	struct {
		int left : 15;
		int right : 15;
		int status : 2;
	} s;
	int i;
} deque_stat_t;

typedef struct node_type {
	deque_stat_t stat;
	void * data;
} node_t;

struct deque_
{
	deque_stat_t stat;

	node_t ** nodes;
	size_t size;
};

static int deque_node_alloc(deque_t * handle)
{
	node_t * exp;
	node_t * new_node;
	size_t s;

	new_node = smr_malloc(sizeof(node_t));
	assert(new_node != NULL);
	new_node->data = NULL;
	new_node->stat.s.left = -1;
	new_node->stat.s.right = -1;
	new_node->stat.s.status = STABLE;
	for (s = 0; s < handle->size; s++)
	{
		exp = NULL;
		if (compare_and_exchange_ptr(&exp, &(handle->nodes[s]), new_node) == 0)
			return s;
	}
	free(new_node);

	return -1;
}

static void deque_node_free(deque_t * handle, int n)
{
	node_t * node = NULL;
	atomic_swap_ptr(&node, &(handle->nodes[n]));
	smr_free(node);
}

deque_t * deque_new(size_t size)
{
	assert(size < 32768);
	deque_t * retval = malloc(sizeof(deque_t));
	assert(retval != NULL);
	retval->nodes = calloc(size, sizeof(node_t*));
	assert(retval->nodes != NULL);

	retval->size = size;
	retval->stat.s.left = -1;
	retval->stat.s.right = -1;
	retval->stat.s.status = STABLE;

	return retval;
}

void deque_free(deque_t * handle)
{
	free(handle->nodes);
	free(handle);
}

void deque_stabilize_front(deque_t * handle, deque_stat_t stat)
{
	// HERE!!
}

void deque_stabilize_back(deque_t * handle, deque_stat_t stat)
{
	int prev;
	int prevnext;
	deque_stat_t new_stat;
	deque_stat_t exp_stat;
	node_t * prevnext_node;
	node_t right_node;

	// register a hazardous reference on the right-most (back) node
	do
	{
		right_node = handle->nodes[stat.s.right];
		hptr_register(0, right_node);
	} while (right_node != handle->nodes[stat.s.right]);
	// register a hazardous reference on the node just left of it
	prev = right_node->stat.s.left;
	do
	{
		prevnode = handle->nodes[prev];
		hptr_register(1, prevnode);
	} while (prevnode != handle->nodes[prev]);
	// check that the state of the deque hasn't changed
	if (handle.stat.i != stat.i)
		return;
	// see whether the node on the right-hand side of the node on the left-hand
	// side of the right-most node is the right-most node
	// if not, the queue is not "stable"
	prevnext = prevnode->stat.s.right;
	if (prevnext != stat.s.right)
	{	// the deque is not stable, so we need to stabilize
		// check if the state of the queue is still the same
		if (handle->stat.i != stat.i)
			return;
		
		int prevprev = prevnode->stat.s.left;
		new_stat.s.left = prevprev;
		new_stat.s.right = stat.s.right;
		new_stat.s.status = stat.s.status;
		exp_stat.s.left = prevprev;
		exp_stat.s.right = prevnext;
		exp_stat.s.status = prevnode->stat.s.status;
		if (compare_and_exchange_int(&(exp_stat.i), &(prevnode->stat.i), newstat.i) != 0)
			return;
	}
	// the queue is stable - tag it as such
	new_stat.s.left = stat.s.left;
	new_stat.s.right = stat.s.right;
	new_stat.s.status = STABLE;
	compare_and_exchange_int(&(stat.i), &(handle->stat.i), new_stat.i);
}

void deque_stabilize(deque_t * handle, deque_stat_t stat)
{
	switch (stat.s.status)
	{
	case LPUSH :
		deque_stabilize_front(handle, stat);
		break;
	case RPUSH :
		deque_stabilize_back(handle, stat);
		break;
	default :
		break;
	}
}

void deque_push_back(deque_t * handle, void * val)
{
	deque_stat_t stat;
	deque_stat_t new_stat;
	int n = deque_node_alloc(handle);
	
	handle->nodes[n]->data = val;
	while (1)
	{
		stat.i = handle->stat.i;
		if (stat.s.right == -1)
		{
			new_stat.s.left = n;
			new_stat.s.right = n;
			new_stat.s.status = stat.s.status;
			if (compare_and_exchange_int(&(stat.i), &(handle->stat.i), new_stat.i) == 0)
				return;
		}
		else if (stat.s.status == STABLE)
		{
			new_stat.s.left = stat.s.right;
			new_stat.s.status = RPUSH;
			if (compare_and_exchange_int(&(stat.i), &(handle->stat.i), new_stat.i) == 0)
			{
				deque_stabilize_back(handle, new_stat);
				return;
			}
		}
		else deque_stabilize(handle, stat);
	}
}
