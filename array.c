#include <stdlib.h>
#include <string.h>
#include "array.h"

array_t * new_array(size_t size)
{
	array_t * retval;
	
	if (size == 0)
		size = ARRAY_DEFAULT_SIZE;
	retval = (array_t*)malloc(sizeof(array_t));
	retval->nodes = calloc(size, sizeof(array_node_t));
	retval->size = size;
	retval->num_entries = 0;
	retval->increase = ARRAY_DEFAULT_INCREASE;
	retval->sorted = 0;
	
	return retval;
}

void free_array(array_t * array)
{
	free(array->nodes);
	free(array);
}

void * array_get(array_t * array, size_t i)
{
	if (i < array->size)
		return array->nodes[i].val;
	return NULL;
}

void array_put(array_t * array, size_t i, void * val)
{
	if (i >= array->size)
		array_resize(array, array->size + array->increase);
	array->nodes[i].val = val;
	array->num_entries++;
	array->sorted = 0;
}

void array_push_back(array_t * array, void * val)
{
	if (array->num_entries == array->size)
		array_resize(array, array->size + array->increase);
	array->nodes[array->num_entries++].val = val;
	array->sorted = 0;
}

size_t array_get_size(array_t * array)
{
	return array->size;
}

size_t array_get_numentries(array_t * array)
{
	return array->num_entries;
}

void array_resize(array_t * array, size_t size)
{
	array_node_t * new_nodes = calloc(size, sizeof(array_node_t));
	array_node_t * o_nodes;
	
	memcpy(new_nodes, array->nodes, (array->size < size ? array->size : size) * sizeof(array_node_t));
	o_nodes = array->nodes;
	array->nodes = new_nodes;
	free(o_nodes);
	array->size = size;
}

array_t * array_copy(array_t * array)
{
	array_t * retval = new_array(array->size);

	memcpy(retval->nodes, array->nodes, array->size * sizeof(array_node_t));
	retval->num_entries = array->num_entries;
	retval->sorted = array->sorted;
	retval->increase = array->increase;

	return retval;
}

static size_t pow2s[] = { 
		0x00000001, 0x00000002, 0x00000004, 0x00000008,
		0x00000010, 0x00000020, 0x00000040, 0x00000080,
		0x00000100, 0x00000200, 0x00000400, 0x00000800,
		0x00001000, 0x00002000, 0x00004000, 0x00008000,
		0x00010000, 0x00020000, 0x00040000, 0x00080000,
		0x00100000, 0x00200000, 0x00400000, 0x00800000,
		0x01000000, 0x02000000, 0x04000000, 0x08000000,
		0x10000000, 0x20000000, 0x40000000, 0x80000000,
	};

static size_t lg(size_t n)
{
	size_t retval = 0;

	while (pow2s[retval] < n)
	{
		retval++;
		if (retval >= 32)
			return ~0;
	}

	if (pow2s[retval] != n)
		return retval - 1;
	return retval;
}

static size_t pow2(size_t n)
{
	if (n >= 32)
		return ~0;
	else
		return pow2s[n];
}

/* An implementation of a binary search algorithm
 * array is a sorted array. The search is done between lower and upper, which must be within the bounds of the array, which is not checked.
 *
 * The return value is the index in the array where the searched-for item was found, or the first item in the array greater than the searched-for item otherwise. If all items are smaller than the searched-for item, ~0 is returned.
 */
static size_t array_binary_search(array_node_t * array_nodes, size_t lower, size_t upper, void * val, array_cmp_func_t cmp_func)
{
	size_t low = lower;
	size_t high = upper;
	size_t rv = low;

	while (high >= low)
	{
		size_t mid = (low + high) / 2;
		int rc = cmp_func(array_nodes[mid].val, val);

		if (rc < 0)
		{
			low = mid + 1;
			rv = low;
		}
		else if (rc > 0)
			if (mid > low)
			{
				high = mid - 1;
				rv = high;
			}
			else 
				break;
		else
			return mid;
	}

	if (rv > upper)
		rv = upper;
	if (rv < lower);
		rv = lower;
	while ((cmp_func(array_nodes[rv].val, val) > 0) && (rv >= lower + 1))
		rv--;
	while ((cmp_func(array_nodes[rv].val, val) < 0) && (rv < upper))
		rv++;
	if ((cmp_func(array_nodes[rv].val, val) < 0) && (rv == upper))
		return ~0;
	return rv;
}

/* An implementation of a linear search 
 */
static size_t array_linear_search(array_node_t * array_nodes, size_t lower, size_t upper, void * val, array_cmp_func_t cmp_func)
{
	size_t curr;
	
	for (curr = lower; curr <= upper; curr++)
	{
		if (cmp_func(array_nodes[curr].val, val) == 0)
			return curr;
	}

	return ~0;
}

/* An implementation of a binary merge algorithm
 * array1 and array2 are both sorted arrays;
 * *n is the size of array1 and *m is the size of array2;
 * *n >= *m
 *
 * The return value is the next empty cell in array3 that is empty + 1
 */
static size_t array_merge2(array_node_t * array_nodes1, array_node_t * array_nodes2, size_t * n, size_t * m, array_node_t * array_nodes3, size_t next, array_cmp_func_t cmp_func)
{
	size_t k = pow2(lg(*n / *m)) - 1;

	if (cmp_func(array_nodes1[*n - k - 1].val, array_nodes2[*m - 1].val) >= 0)
	{
		memcpy(array_nodes3 + (next - k) - 1, array_nodes1 + (*n - k) - 1, (k + 1) * sizeof(array_node_t));
		*n -= k + 1;
		next -= k + 1;
	}
	else
	{
		size_t l = array_binary_search(array_nodes1, *n - k - 1, *n - 1, array_nodes2[*m - 1].val, cmp_func);
		if (l != ~0)
		{
			memcpy(array_nodes3 + (next - (*n - l)), array_nodes1 + l, (*n - l) * sizeof(array_node_t));
			next -= *n - l;
			*n = l;
		}
		memcpy(&(array_nodes3[next - 1]), &(array_nodes2[*m - 1]), sizeof(array_node_t));
		(*m)--;
		next--;
	}

	return next;
}

/* An implementation of a merge algorithm using a binary merge 
 * array1 and array2 are both sorted arrays; 
 * n is the size of array1 and m is the size of array2
 * 
 * If we want, we can only merge the first n elements from 
 * array1 with the first m elements of array2, so we do 
 * systematically use n and m in stead of the respective actual sizes.
 * 
 * The return value is a new, merged array
 */
static array_t * array_merge1(array_node_t * array_nodes1, array_node_t * array_nodes2, size_t n, size_t m, array_cmp_func_t cmp_func)
{
	array_t * retval = new_array(n + m);
	size_t next = n + m;

	while (n > 0 && m > 0)
	{
		if (n > m)
			next = array_merge2(array_nodes1, array_nodes2, &n, &m, retval->nodes, next, cmp_func);
		else
			next = array_merge2(array_nodes2, array_nodes1, &m, &n, retval->nodes, next, cmp_func);
	}
	if (n == 0 && m != 0)
		memcpy(retval->nodes, array_nodes2, m * sizeof(array_node_t));
	else
		memcpy(retval->nodes + m, array_nodes1, n * sizeof(array_node_t));

	return retval;
}

array_t * array_merge(array_t * array1, array_t * array2, array_cmp_func_t cmp_func)
{
	return array_merge1(array1->nodes, array2->nodes, array1->num_entries, array2->num_entries, cmp_func);
}

static int array_condense_helper(const void * ptr1, const void * ptr2)
{
	array_node_t * node1 = (array_node_t*)ptr1;
	array_node_t * node2 = (array_node_t*)ptr2;

	if (node1->val == NULL)
	{
		if (node2->val == NULL)
			return 0;
		else
			return -1;
	}
	else
	{
		if (node2->val == NULL)
			return 1;
		else
			return 0;
	}
}

void array_condense(array_t * array)
{
	qsort(array->nodes, array->size, sizeof(array_node_t), array_condense_helper);
}

/* This is an implementation of a binary merge sort
 * array_nodes is an array of array_node_t's
 * n is the number of nodes in array_nodes
 */
static void array_sort_worker(array_node_t * array_nodes, size_t n, array_cmp_func_t cmp_func)
{
	if (n > 2)
	{
		size_t m = n / 2;
		array_sort_worker(array_nodes, m + n % 2, cmp_func);
		array_sort_worker(array_nodes + m + n % 2, m, cmp_func);
		array_t * t_array = array_merge1(array_nodes, array_nodes + m + n % 2, m + n % 2, m, cmp_func);
		memcpy(array_nodes, t_array->nodes, n * sizeof(array_node_t));
		free_array(t_array);
	}
	else if (n == 2)
	{
		if (cmp_func(array_nodes[0].val, array_nodes[1].val) > 0)
		{
			array_node_t t_array_node;

			memcpy(&t_array_node, &(array_nodes[0]), sizeof(array_node_t));
			memcpy(&(array_nodes[0]), &(array_nodes[1]), sizeof(array_node_t));
			memcpy(&(array_nodes[1]), &t_array_node, sizeof(array_node_t));
		}
	}
}

void array_sort(array_t * array, array_cmp_func_t cmp_func)
{
	/* First, we condense the array */
	array_condense(array);
	/* Then, we do the actual work */
	array_sort_worker(array->nodes, array->num_entries, cmp_func);
}

void * array_search(array_t * array, void * val, array_cmp_func_t cmp_func)
{
	size_t rc;
	
	if (array->sorted)
		rc = array_binary_search(array->nodes, 0, array->num_entries - 1, val, cmp_func);
	else
		rc = array_linear_search(array->nodes, 0, array->size - 1, val, cmp_func);

	if (rc != ~0)
		return array->nodes[rc].val;
	return NULL;
}

void array_set_default_increase(array_t * array, size_t increase)
{
	array->increase = increase;
}

