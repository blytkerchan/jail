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

static int node_alloc(deque_t * handle)
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

static void node_free(deque_t * handle, int n)
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

void deque_stabilize(deque_t * handle, deque_stat_t stat)
{
	// HERE!!
}

void deque_stabilize_back(deque_t * handle, deque_stat_t stat)
{
	// HERE!!
}

void deque_push_back(deque_t * handle, void * val)
{
	deque_stat_t stat;
	deque_stat_t new_stat;
	int n = node_alloc(handle);
	
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
