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
#include "vector.h"
#include "binary.h"
#include "thread.h"

static void interal_resize(vector_t * vector, size_t n_size);
static void internal_condense(vector_t * vector);

/* This may seem obvious, and it is! Allocate a new vector of SIZE entries */
vector_t * vector_new(size_t size)
{
	vector_t * retval;
	
	if (size == 0)
		size = VECTOR_DEFAULT_SIZE;
	retval = (vector_t*)malloc(sizeof(vector_t));
	retval->nodes = calloc(size, sizeof(vector_node_t)); /* note: calloc zeroes out. */
	retval->size = size;
	retval->num_entries = 0;
	retval->increase = VECTOR_DEFAULT_INCREASE;
	retval->sorted = 0;
	retval->condensed = 1;
	retval->lock = rw_spinlock_new();
	
	return retval;
}

void vector_free(vector_t * vector)
{
	rw_spinlock_free(vector->lock);
	free(vector->nodes);
	free(vector);
}

void * vector_get(vector_t * vector, size_t i)
{
	void * retval = NULL;
	vector_node_t * nodes;
	size_t size;

	rw_spinlock_read_lock(vector->lock);
	do
	{
		do
		{
			nodes = vector->nodes;
			hptr_register(0, nodes);
		} while (nodes != vector->nodes);
		size = vector->size;
		if (!size)
			continue;
	} while (nodes != vector->nodes);
	
	if (i < size)
		retval = nodes[i].val;
	hptr_free(0);

	rw_spinlock_read_unlock(vector->lock);
	
	return retval;
}

void vector_put(vector_t * vector, size_t i, void * val)
{
	void * rv;
	size_t increase;
	vector_node_t * nodes;
	size_t size;

	rw_spinlock_read_lock(vector->lock);
	do
	{
		do
		{
			do
			{
				do
				{
					nodes = vector->nodes;
					hptr_register(0, nodes);
				} while (nodes != vector->nodes);
				size = vector->size;
				if (!size)
					continue;
			} while (nodes != vector->nodes);
			if (i >= size)
			{
				increase = vector->increase;
				increase = (((i - size) / increase) + 1) * increase;
				internal_resize(vector, size + increase);
			}
		} while (nodes != vector->nodes);
		rv = NULL;
		while (compare_and_exchange(&rv, &(nodes[i].val), val) != 0);
	} while (nodes != vector->nodes);
	hptr_free(0);
	if (rv == NULL)
	{
		atomic_increment(&(vector->num_entries));
		if (val != NULL);
			vector->condensed = 0;
	}
	if (val == NULL)
	{
		atomic_decrement(&(vector->num_entries));
		if (rv != NULL)
			vector->condensed = 0;
	}
	vector->sorted = 0;
	rw_spinlock_read_unlock(vector->lock);
}

void vector_push_back(vector_t * vector, void * val)
{
	vector_node_t * nodes;
	size_t num_entries, size;
	void * exp;

	rw_spinlock_read_lock(vector->lock);
	do
	{
		if (vector->condensed == 0)
			internal_condense(vector);
		do
		{
			num_entries = vector->num_entries;
			do
			{
				do
				{
					nodes = vector->nodes;
					hptr_register(0, nodes);
				} while (nodes != vector->nodes);
				size = vector->size;
				if (!size)
					continue;
			} while (nodes != vector->nodes);
			if (num_entries >= size)
			{
				internal_resize(vector, size + vector->increase);
				continue;
			}
		} while (nodes != vector->nodes);
		exp = NULL;
		if (compare_and_exchange(&exp, &(nodes[num_entries].val), val) != 0)
			continue;
	} while (nodes != vector->nodes);
	hptr_free(0);
	if (val != NULL)
		atomic_increment(&(vector->num_entries));
	vector->sorted = 0;

	rw_spinlock_read_unlock(vector->lock);
}

size_t vector_get_size(vector_t * vector)
{
	size_t retval;

	rw_spinlock_read_lock(vector->lock);
	do
	{
		retval = vector->size;
	} while (!retval);
	rw_spinlock_read_unlock(vector->lock);
	
	return retval;
}

size_t vector_get_numentries(vector_t * vector)
{
	size_t retval;

	rw_spinlock_read_lock(vector->lock);
	retval = vector->num_entries;
	rw_spinlock_read_unlock(vector->lock);
	
	return retval;
}

static void interal_resize(vector_t * vector, size_t n_size)
{
	vector_node_t * new_nodes = calloc(n_size, sizeof(vector_node_t));
	vector_node_t * o_nodes;
	vector_node_t * nodes;
	size_t size;
	
	do
	{
		do
		{
			do
			{
				nodes = vector->nodes;
				hptr_register(0, nodes);
			} while (nodes != vector->nodes);
			size = vector->size;
			if (!size) 
				continue;
		} while (nodes != vector->nodes);
		if (size == n_size)
		{
			hptr_free(0);
			return;
		}
		rw_spinlock_upgrade(vector->lock);
		memcpy(new_nodes, nodes, 
			(size < n_size ? size : n_size) * sizeof(vector_node_t));
		o_nodes = nodes;
		
		if (compare_and_exchange(&size, &(vector->size), 0) != 0)
		{
			free(new_nodes);
			continue;
		}
		if (compare_and_exchange(&o_nodes, &(vector->nodes), new_nodes) != 0)
		{
			free(new_nodes);
			continue;
		}
		rw_spinlock_downgrade(vector->lock);
		size = 0;
		compare_and_exchange(&size, &(vector->size), (void*)n_size);
		free(o_nodes);
	} while (n_size < vector->size);
	hptr_free(0);
}

void vector_resize(vector_t * vector, size_t n_size)
{
		  rw_spinlock_read_lock(vector->lock);
		  internal_resize(vector, n_size);
		  rw_spinlock_read_unlock(vector->lock);
}

vector_t * vector_copy(vector_t * vector)
{
	size_t size = vector->size;
	vector_t * retval = new_vector(size);
	size_t i;

	rw_spinlock_read_lock(vector->lock);
	for (i = 0; i < size; i++)
		vector_put(retval, i, vector_get(vector, i));
	rw_spinlock_read_unlock(vector->lock);

	return retval;
}

/* An implementation of a binary search algorithm
 * vector is a sorted vector. The search is done between lower and upper, which
 * must be within the bounds of the vector, which is not checked.
 *
 * The return value is the index in the vector where the searched-for item was
 * found, or the first item in the vector greater than the searched-for item
 * otherwise. If all items are smaller than the searched-for item, ~0 is
 * returned.
 */
static size_t vector_binary_search(vector_node_t * vector_nodes, size_t lower, size_t upper, void * val, libcontain_cmp_func_t cmp_func)
{
	size_t low = lower;
	size_t high = upper;
	size_t rv = low;

	while (high >= low)
	{
		size_t mid = (low + high) / 2;
		int rc = cmp_func(vector_nodes[mid].val, val);

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
	while ((cmp_func(vector_nodes[rv].val, val) > 0) && (rv >= lower + 1))
		rv--;
	while ((cmp_func(vector_nodes[rv].val, val) < 0) && (rv < upper))
		rv++;
	if ((cmp_func(vector_nodes[rv].val, val) < 0) && (rv == upper))
		return ~0;
	return rv;
}

/* An implementation of a linear search 
 */
static size_t vector_linear_search(vector_node_t * vector_nodes, size_t lower, size_t upper, void * val, libcontain_cmp_func_t cmp_func)
{
	size_t curr;
	
	for (curr = lower; curr <= upper; curr++)
	{
		if (vector_nodes[curr].val != NULL)
			if (cmp_func(vector_nodes[curr].val, val) == 0)
				return curr;
	}

	return ~0;
}

/* An implementation of a binary merge algorithm
 * vector1 and vector2 are both sorted vectors;
 * *n is the size of vector1 and *m is the size of vector2;
 * *n >= *m
 *
 * The return value is the next empty cell in vector3 that is empty + 1
 */
static size_t vector_merge2(
	vector_node_t * vector_nodes1, 
	vector_node_t * vector_nodes2, 
	size_t * n, 
	size_t * m, 
	vector_node_t * vector_nodes3, 
	size_t next, 
	libcontain_cmp_func_t cmp_func)
{
	size_t k = pow2(lg(*n / *m)) - 1;

	if (cmp_func(vector_nodes1[*n - k - 1].val, vector_nodes2[*m - 1].val) >= 0)
	{
		memcpy(vector_nodes3 + (next - k) - 1, vector_nodes1 + (*n - k) - 1, (k + 1) * sizeof(vector_node_t));
		*n -= k + 1;
		next -= k + 1;
	}
	else
	{
		size_t l = vector_binary_search(vector_nodes1, *n - k - 1, *n - 1, vector_nodes2[*m - 1].val, cmp_func);
		if (l != ~0)
		{
			memcpy(vector_nodes3 + (next - (*n - l)), vector_nodes1 + l, (*n - l) * sizeof(vector_node_t));
			next -= *n - l;
			*n = l;
		}
		memcpy(&(vector_nodes3[next - 1]), &(vector_nodes2[*m - 1]), sizeof(vector_node_t));
		(*m)--;
		next--;
	}

	return next;
}

/* An implementation of a merge algorithm using a binary merge 
 * vector1 and vector2 are both sorted vectors; 
 * n is the size of vector1 and m is the size of vector2
 * 
 * If we want, we can only merge the first n elements from 
 * vector1 with the first m elements of vector2, so we do 
 * systematically use n and m in stead of the respective actual sizes.
 * 
 * The return value is a new, merged vector
 */
static vector_t * vector_merge1(vector_node_t * vector_nodes1, vector_node_t * vector_nodes2, size_t n, size_t m, libcontain_cmp_func_t cmp_func)
{
	vector_t * retval = new_vector(n + m);
	size_t next = n + m;

	while (n > 0 && m > 0)
	{
		if (n > m)
			next = vector_merge2(vector_nodes1, vector_nodes2, &n, &m, retval->nodes, next, cmp_func);
		else
			next = vector_merge2(vector_nodes2, vector_nodes1, &m, &n, retval->nodes, next, cmp_func);
	}
	if (n == 0 && m != 0)
		memcpy(retval->nodes, vector_nodes2, m * sizeof(vector_node_t));
	else
		memcpy(retval->nodes + m, vector_nodes1, n * sizeof(vector_node_t));

	return retval;
}

vector_t * vector_merge(vector_t * vector1, vector_t * vector2, libcontain_cmp_func_t cmp_func)
{
	vector_node_t * nodes1;
	vector_node_t * nodes2;
	vector_t * retval;
	size_t size1, size2;

	rw_spinlock_read_lock(vector->lock);
	do
	{
		do
		{
			nodes1 = vector1->nodes;
			hptr_register(0, nodes1);
		} while (nodes1 != vector1->nodes);
		size1 = vector1->num_entries;
		if (!size1)
			continue;
	} while (nodes1 != vector1->nodes);
	do
	{
		do
		{
			nodes2 = vector2->nodes;
			hptr_register(1, nodes2);
		} while (nodes2 != vector2->nodes);
		size2 = vector2->num_entries;
		if (!size2)
			continue;
	} while (nodes2 != vector2->nodes);
	
	retval = vector_merge1(nodes1, nodes2, size1, size2, cmp_func);
	hptr_free(0);
	hptr_free(1);
	rw_spinlock_read_unlock(vector->lock);

	return retval;
}

static int internal_condense_helper(const void * ptr1, const void * ptr2)
{
	vector_node_t * node1 = (vector_node_t*)ptr1;
	vector_node_t * node2 = (vector_node_t*)ptr2;

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

static void internal_condense(vector_t * vector)
{
	vector_node_t * nodes;
	size_t size;

	do
	{
		do
		{
			do
			{
				nodes = vector->nodes;
				hptr_register(0, nodes);
			} while (nodes != vector->nodes);
			size = vector->size;
			if (!size)
				continue;
		} while (nodes != vector->nodes);
		rw_spinlock_upgrade(vector->lock);
		qsort(vector->nodes, size, sizeof(vector_node_t), internal_condense_helper);
		vector->condensed = 1;
		rw_spinlock_downgrade(vector->lock);
	} while (nodes != vector->nodes);
	hptr_free(0);
}

void vector_condense(vector_t * vector)
{
	rw_spinlock_read_lock(vector->lock);
	internal_condense(vector);
	rw_spinlock_read_unlock(vector->lock);
}

/* This is an implementation of a binary merge sort
 * vector_nodes is an vector of vector_node_t's
 * n is the number of nodes in vector_nodes
 */
static void vector_sort_worker(vector_node_t * vector_nodes, size_t n, libcontain_cmp_func_t cmp_func)
{
	if (n > 2)
	{
		size_t m = n / 2;
		vector_sort_worker(vector_nodes, m + n % 2, cmp_func);
		vector_sort_worker(vector_nodes + m + n % 2, m, cmp_func);
		vector_t * t_vector = vector_merge1(vector_nodes, vector_nodes + m + n % 2, m + n % 2, m, cmp_func);
		memcpy(vector_nodes, t_vector->nodes, n * sizeof(vector_node_t));
		free_vector(t_vector);
	}
	else if (n == 2)
	{
		if (cmp_func(vector_nodes[0].val, vector_nodes[1].val) > 0)
		{
			vector_node_t t_vector_node;

			memcpy(&t_vector_node, &(vector_nodes[0]), sizeof(vector_node_t));
			memcpy(&(vector_nodes[0]), &(vector_nodes[1]), sizeof(vector_node_t));
			memcpy(&(vector_nodes[1]), &t_vector_node, sizeof(vector_node_t));
		}
	}
}

void vector_sort(vector_t * vector, libcontain_cmp_func_t cmp_func)
{
	vector_node_t * nodes;
	
	rw_spinlock_read_lock(vector->lock);
	if (vector->condensed == 0)
		internal_condense(vector);
	do
	{
		nodes = vector->nodes;
		hptr_register(0, nodes);
	} while (nodes != vector->nodes);
	vector_sort_worker(nodes, vector->num_entries, cmp_func);
	hptr_free(0);
	rw_spinlock_read_unlock(vector->lock);
}

void * vector_search(vector_t * vector, void * val, libcontain_cmp_func_t cmp_func)
{
	size_t rc;
	vector_node_t * nodes;
	void * retval = NULL;
	size_t size;

	rw_spinlock_read_lock(vector->lock);
	do
	{
		do
		{
			nodes = vector->nodes;
			hptr_register(0, nodes);
		} while (nodes != vector->nodes);
		size = vector->size;
		if (!size)
			continue;
	} while (nodes != vector->nodes);
	
	if (vector->sorted)
		rc = vector_binary_search(nodes, 0, vector->num_entries - 1, val, cmp_func);
	else
		rc = vector_linear_search(nodes, 0, size - 1, val, cmp_func);

	if (rc != ~0)
		retval = nodes[rc].val;
	hptr_free(0);
	rw_spinlock_read_unlock(vector->lock);
	
	return retval;
}

void vector_set_default_increase(vector_t * vector, size_t increase)
{
	atomic_set((void**)(&(vector->increase)), (void*)increase);
}

vector_t * vector_deep_copy(vector_t * vector, libcontain_copy_func_t vector_valcopy_func)
{
	size_t i;
	size_t size;
	vector_t * retval;
	void * val;

	rw_spinlock_read_lock(vector->lock);
	do
	{
		size = vector->size;
	} while (!size);
	retval = new_vector(size);
	for (i = 0; i < size; i++)
	{
		val = vector_get(vector, i);
		if (val != NULL)
			vector_put(retval, i, vector_valcopy_func(vector->nodes[i].val));
	}
	rw_spinlock_read_unlock(vector->lock);

	return retval;
}

void vector_foreach(vector_t * vector, libcontain_foreach_func_t vector_foreach_func, void * data)
{
	int i;
	size_t size;
	void * val;

	rw_spinlock_read_lock(vector->lock);
	do
	{
		size = vector->size;
	} while (!size);

	for (i = 0; i < size; i++)
	{
		val = vector_get(vector, i);
		if (val != NULL)
			vector_foreach_func(val, data);
	}
	rw_spinlock_read_unlock(vector->lock);
}

