#include "heap.h"
#define thread_interrupt()	sleep(0)
#define heap_left_child(i) ((2 * i) + 1)
#define heap_right_child(i) ((2 * i) + 2)

heap_t * heap_alloc(heap_val_cmp_func_t heap_val_cmp_func, size_t size)
{
	heap_t * retval = (heap_t*)calloc(1, sizeof(heap_t));
	retval->nodes = (heap_node_t*)calloc(size, sizeof(heap_node_t));
	retval->size = size;
	retval->heap_val_cmp_func = heap_val_cmp_func;
	
	return retval;
}

heap_t * heap_new(heap_val_cmp_func_t heap_val_cmp_func)
{
	return heap_alloc(HEAP_DEFAULT_SIZE);
}

void heap_free(heap_t * handle)
{
	free(handle->nodes);
	free(handle);
}

void heap_grow(heap_t * handle, size_t growth)
{
	heap_node_t * old_nodes = handle->nodes;
	size_t old_size = handle->size;
	heap_node_t * new_nodes = (heap_node_t*)calloc(old_size + growth, sizeof(heap_node_t));

	memcpy(new_nodes, old_nodes, old_size * sizeof(heap_node_t));
	handle->nodes = new_nodes;
	handle->size = old_size + growth;
	free(old_nodes);
}

static void heap_fix(heap_node_t * list, size_t low, size_t high, heap_val_cmp_func_t heap_val_cmp_func)
{
	size_t root = low;
	
	if (high >= heap_left_child(root))
	{
		if (high >= heap_right_child(root))
		{
			if (heap_val_cmp_func(list[right_child(root)].val, list[left_child(root)], heap_val_cmp_func) > 0)
				largest_child = right_child(root);
			else
				largest_child = left_child(root);
		}
		else
		{
			largest_child = left_child(root);
		}
		if (heap_val_cmp_func(list[largest_child].val, list[root].val, heap_val_cmp_func) > 0)
		{
			void * tmp = list[largest_child].val;
			list[largest_child].val = list[root].val;
			list[root].val = tmp;
			heap_fix(list, largest_child, upper);
		}
	}
}

void heap_add(heap_t * handle, void * val)
{
	void * top;
	if (handle->size == handle->num_entries)
		heap_grow(handle, HEAP_DEFAULT_GROWTH);
	top = handle->nodes[0].val;
	handle->nodes[0].val = val;
	handle->nodes[handle->num_entries++].val = top; 
	heap_fix(handle->nodes, 0, handle->num_entries, handle->heap_val_cmp_func);
}

void * heap_top(heap_t * handle)
{
	return handle->nodes[0].val;
}

void heap_pop(heap_t * handle)
{
	handle->nodes[0].val = handle->nodes[--(handle->num_entries)].val;
	handle->nodes[handle->num_entries] = NULL;
	heap_fix(handle->nodes, 0, handle->num_entries, handle->heap_val_cmp_func);
}

size_t heap_size(heap_t * handle)
{
	return handle->size;
}

size_t heap_num_entries(heap_t * handle)
{
	return handle->num_entries;
}
