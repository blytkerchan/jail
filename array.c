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
#include <string.h>
#include <libmemory/smr.h>
#include <libmemory/hptr.h>
#include "arch/include/set.h"
#include "arch/include/compare_and_exchange.h"
#include "arch/include/increment.h"
#include "arch/include/decrement.h"
#include "array.h"
#include "binary.h"
#include "thread.h"

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
	retval->condensed = 1;
	
	return retval;
}

void free_array(array_t * array)
{
	free(array->nodes);
	free(array);
}

void * array_get(array_t * array, size_t i)
{
	void * retval = NULL;
	array_node_t * nodes;
	size_t size;

	do
	{
		do
		{
			nodes = array->nodes;
			hptr_register(0, nodes);
		} while (nodes != array->nodes);
		size = array->size;
		if (!size)
			continue;
	} while (nodes != array->nodes);
	
	if (i < size)
		retval = array->nodes[i].val;
	hptr_free(0);

	return NULL;
}

void array_put(array_t * array, size_t i, void * val)
{
	void * rv;
	size_t increase;
	array_node_t * nodes;
	size_t size;

	do
	{
		do
		{
			do
			{
				do
				{
					nodes = array->nodes;
					hptr_register(0, nodes);
				} while (nodes != array->nodes);
				size = array->size;
				if (!size)
					continue;
			} while (nodes != array->nodes);
			if (i >= size)
			{
				increase = array->increase;
				increase = (((i - size) / increase) + 1) * increase;
				array_resize(array, size + increase);
			}
		} while (nodes != array->nodes);
		rv = NULL;
		while (compare_and_exchange(&rv, &(nodes[i].val), val) != 0);
	} while (nodes != array->nodes);
	hptr_free(0);
	if (rv == NULL)
	{
		atomic_increment(&(array->num_entries));
		if (val != NULL);
			array->condensed = 0;
	}
	if (val == NULL)
	{
		atomic_decrement(&(array->num_entries));
		if (rv != NULL)
			array->condensed = 0;
	}
	array->sorted = 0;
}

void array_push_back(array_t * array, void * val)
{
	array_node_t * nodes;
	size_t num_entries, size;
	void * exp;
	
	do
	{
		if (array->condensed == 0)
			array_condense(array);
		do
		{
			num_entries = array->num_entries;
			do
			{
				do
				{
					nodes = array->nodes;
					hptr_register(0, nodes);
				} while (nodes != array->nodes);
				size = array->size;
				if (!size)
					continue;
			} while (nodes != array->nodes);
			if (num_entries >= size)
			{
				array_resize(array, size + array->increase);
				continue;
			}
		} while (nodes != array->nodes);
		exp = NULL;
		if (compare_and_exchange(&exp, &(nodes[num_entries].val), val) != 0)
			continue;
	} while (nodes != array->nodes);
	hptr_free(0);
	if (val != NULL)
		atomic_increment(&(array->num_entries));
	array->sorted = 0;
}

size_t array_get_size(array_t * array)
{
	size_t retval;

	do
	{
		retval = array->size;
	} while (!retval);
	
	return retval;
}

size_t array_get_numentries(array_t * array)
{
	return array->num_entries;
}

void array_resize(array_t * array, size_t n_size)
{
	array_node_t * new_nodes = calloc(n_size, sizeof(array_node_t));
	array_node_t * o_nodes;
	array_node_t * nodes;
	size_t size;
	
	do
	{
		do
		{
			do
			{
				nodes = array->nodes;
				hptr_register(0, nodes);
			} while (nodes != array->nodes);
			size = array->size;
			if (!size) 
				continue;
		} while (nodes != array->nodes);
		if (size == n_size)
		{
			hptr_free(0);
			return;
		}
		/* FIXME: race condition starts here */
		memcpy(new_nodes, nodes, 
			(size < n_size ? size : n_size) * sizeof(array_node_t));
		o_nodes = nodes;
		
		if (compare_and_exchange(&size, &(array->size), 0) != 0)
		{
			free(new_nodes);
			continue;
		}
		/* FIXME: race condition ends after the following line */
		if (compare_and_exchange(&o_nodes, &(array->nodes), new_nodes) != 0)
		{
			free(new_nodes);
			continue;
		}
		size = 0;
		compare_and_exchange(&size, &(array->size), (void*)n_size);
		free(o_nodes);
	} while (n_size < array->size);
	hptr_free(0);
}

array_t * array_copy(array_t * array)
{
	size_t size = array->size;
	array_t * retval = new_array(size);
	size_t i;

	for (i = 0; i < size; i++)
		array_put(retval, i, array_get(array, i));

	return retval;
}

/* An implementation of a binary search algorithm
 * array is a sorted array. The search is done between lower and upper, which
 * must be within the bounds of the array, which is not checked.
 *
 * The return value is the index in the array where the searched-for item was
 * found, or the first item in the array greater than the searched-for item
 * otherwise. If all items are smaller than the searched-for item, ~0 is
 * returned.
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
		if (array_nodes[curr].val != NULL)
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
static size_t array_merge2(
	array_node_t * array_nodes1, 
	array_node_t * array_nodes2, 
	size_t * n, 
	size_t * m, 
	array_node_t * array_nodes3, 
	size_t next, 
	array_cmp_func_t cmp_func)
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
	array_node_t * nodes1;
	array_node_t * nodes2;
	array_t * retval;
	size_t size1, size2;

	do
	{
		do
		{
			nodes1 = array1->nodes;
			hptr_register(0, nodes1);
		} while (nodes1 != array1->nodes);
		size1 = array1->num_entries;
		if (!size1)
			continue;
	} while (nodes1 != array1->nodes);
	do
	{
		do
		{
			nodes2 = array2->nodes;
			hptr_register(1, nodes2);
		} while (nodes2 != array2->nodes);
		size2 = array2->num_entries;
		if (!size2)
			continue;
	} while (nodes2 != array2->nodes);
	
	retval = array_merge1(nodes1, nodes2, size1, size2, cmp_func);
	hptr_free(0);
	hptr_free(1);

	return retval;
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
			return 1;
	}
	else
	{
		if (node2->val == NULL)
			return -1;
		else
			return 0;
	}
}

void array_condense(array_t * array)
{
	array_node_t * nodes;
	size_t size;

	do
	{
		do
		{
			do
			{
				nodes = array->nodes;
				hptr_register(0, nodes);
			} while (nodes != array->nodes);
			size = array->size;
			if (!size)
				continue;
		} while (nodes != array->nodes);
		qsort(array->nodes, size, sizeof(array_node_t), array_condense_helper);
		array->condensed = 1;
	} while (nodes != array->nodes);
	hptr_free(0);
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
	array_node_t * nodes;
	
	if (array->condensed == 0)
		array_condense(array);
	do
	{
		nodes = array->nodes;
		hptr_register(0, nodes);
	} while (nodes != array->nodes);
	array_sort_worker(nodes, array->num_entries, cmp_func);
	hptr_free(0);
}

void * array_search(array_t * array, void * val, array_cmp_func_t cmp_func)
{
	size_t rc;
	array_node_t * nodes;
	void * retval = NULL;
	size_t size;

	do
	{
		do
		{
			nodes = array->nodes;
			hptr_register(0, nodes);
		} while (nodes != array->nodes);
		size = array->size;
		if (!size)
			continue;
	} while (nodes != array->nodes);
	
	if (array->sorted)
		rc = array_binary_search(nodes, 0, array->num_entries - 1, val, cmp_func);
	else
		rc = array_linear_search(nodes, 0, size - 1, val, cmp_func);

	if (rc != ~0)
		retval = nodes[rc].val;
	hptr_free(0);
	
	return retval;
}

void array_set_default_increase(array_t * array, size_t increase)
{
	atomic_set((void**)(&(array->increase)), (void*)increase);
}

array_t * array_deep_copy(array_t * array, array_valcopy_func_t array_valcopy_func)
{
	size_t i;
	size_t size;
	array_t * retval;
	void * val;

	do
	{
		size = array->size;
	} while (!size);
	retval = new_array(size);
	for (i = 0; i < size; i++)
	{
		val = array_get(array, i);
		if (val != NULL)
			array_put(retval, i, array_valcopy_func(array->nodes[i].val));
	}

	return retval;
}

void array_foreach(array_t * array, array_foreach_func_t array_foreach_func, void * data)
{
	int i;
	size_t size;
	void * val;

	do
	{
		size = array->size;
	} while (!size);

	for (i = 0; i < size; i++)
	{
		val = array_get(array, i);
		if (val != NULL)
			array_foreach_func(val, data);
	}
}
