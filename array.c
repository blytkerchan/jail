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

/* This may seem obvious, and it is! Allocate a new array of SIZE entries */
array_t * array_new(size_t size)
{
	array_t * retval;
	
	if (size == 0)
		size = ARRAY_DEFAULT_SIZE;
	retval = (array_t*)malloc(sizeof(array_t));
	retval->nodes = calloc(size, sizeof(array_node_t)); /* calloc zeroes out. */
	retval->size = size;
	retval->num_entries = 0;
	/* if nothing is in the array, there is no disorder in the non-existant 
	 * entries either, so the array is sorted */
	retval->sorted = 1;
	/* if nothing is in the array, there is no space between the (non-existant) 
	 * entries either, so the array is condensed. */
	retval->condensed = 1;
	
	return retval;
}

/* do the obvious */
void array_free(array_t * array)
{
	free(array->nodes);
	free(array);
}

/* get an entry from the array in a thread-safe manner */
void * array_get(array_t * array, size_t i)
{
	void * retval = NULL;
	array_node_t * nodes;

	do
	{
		nodes = array->nodes;
		hptr_register(0, nodes);
	} while (nodes != array->nodes);
	/* we now know nodes == array->nodes and nodes will not be freed until 
	 * we're done */
	
	if (i < array->size)
		retval = nodes[i].val;
	/* we no longer need nodes, so we can free the hazardous reference to it */
	hptr_free(0);

	return retval;
}

/* set or clear a node. Note that the array's attributes, other than the nodes
 * are only updated when the actual set operation is done, so the condensed and
 * num_entries attributes may not reflect the actual state of the array for 
 * some time during the operation of this function */
int array_put(array_t * array, size_t i, void * val)
{
	void * rv;
	array_node_t * nodes;

	do
	{
		do
		{
			nodes = array->nodes;
			hptr_register(0, nodes);
		} while (nodes != array->nodes);
		/* we no know nodes == array->nodes and nodes will not be freed until 
		 * we're done */
		/* we don't resize arrays, so we fail if the write is out-of-bounds */
		if (i >= array->size)
			return -1;
		rv = NULL;
		/* we want to know what value we replaced, in order to know whether the
		 * entry count should be increment and whether we can assume to still be
		 * condensed. */
		while (compare_and_exchange_ptr(&rv, &(nodes[i].val), val) != 0);
	} while (nodes != array->nodes);
	/* when we get here, the node has been set/replaced and we know it is still
	 * linked to the array, so the actual work is done. */
	hptr_free(0);
	
	if (rv == NULL)
	{	/* we replaced an empty node */
		atomic_increment((uint32_t*)&(array->num_entries));
		if (val != NULL)
			array->condensed = 0;
	}
	if (val == NULL)
	{	/* we emptied a node */
		atomic_decrement((uint32_t*)&(array->num_entries));
		if (rv != NULL)
			array->condensed = 0;
	}
	array->sorted = 0;

	return 0;
}

/* get the size attribute of the array. There's really no reason to use a 
 * temporary variable for this, but the compiler will optimize this away 
 * (might even optimize the whole function call away) and it makes the code a
 * wee bit clearer, IMHO. */
size_t array_get_size(array_t * array)
{
	size_t retval;

	retval = array->size;
	
	return retval;
}

/* get the number of entries in the array. Note that this may vary during any 
 * write operations and that that variation is not guaranteed to be during the
 * same time that the write operation itself takes place. Hence, this function
 * is indicative only and does not guarantee to reflect the actual state of 
 * the array if write operations are in progress.
 * As for style (vs. the function above - who said I had to be consistent? */
size_t array_get_numentries(array_t * array)
{
	return array->num_entries;
}

/* This is an enormous monstrosity of a function: it copies each of the values
 * one by one from the old to the new array. There's no way around this, though:
 * because a simple block-copy of the memroy wouldn't be guaranteed to work, as
 * it wouldn't guarantee that the operation is done atomically. */
array_t * array_copy(array_t * array)
{
	size_t size = array->size;
	array_t * retval = array_new(size);
	size_t i;

	for (i = 0; i < size; i++)
		array_put(retval, i, array_get(array, i));

	return retval;
}

/* An implementation of a binary search algorithm
 * array is a sorted array. The search is done between lower and upper, which
 * must be within the bounds of the array.
 *
 * The return value is the index in the array where the searched-for item was
 * found, or the first item in the array greater than the searched-for item
 * otherwise. If all items are smaller than the searched-for item, ~0 is
 * returned.
 */
static size_t array_binary_search(array_node_t * array_nodes, size_t lower, size_t upper, void * val, libcontain_cmp_func_t cmp_func)
{
	size_t low = lower;
	size_t high = upper;
	size_t rv = low;

	/* this loop simulates a recursion (of sorts): the boundaries get closer to
	 * each other until they either meet, or the searched-for value is found. */
	while (high >= low)
	{
		size_t mid = (low + high) / 2;
		/* get the value in the middle of the array and compare it to the one we
		 * want */
		int rc = cmp_func(array_nodes[mid].val, val);

		if (rc < 0)
		{	/* smaller: recurse to the left */
			low = mid + 1;
			rv = low;
		}
		else if (rc > 0)
		{	/* greater: recurse to the right */
			if (mid > low)
			{
				high = mid - 1;
				rv = high;
			}
			else 
				break;
		}
		else
			return mid;
	}

	/* compensate for misses - we return the largest value smaller than the one
	 * requested in that case, or ~0 if there are none greater */
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

/* An implementation of a linear search. This is blindingly obvious code.. */
static size_t array_linear_search(array_node_t * array_nodes, size_t lower, size_t upper, void * val, libcontain_cmp_func_t cmp_func)
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
	libcontain_cmp_func_t cmp_func)
{
	/* K is an offset in ARRAY_NODES1 dependent on N and M, where N >= M. This 
	 * offset is chosen as (more or less) a vantage point in ARRAY_NODES1 from
	 * which to look at the nodes in ARRAY_NODES2. */
	size_t k = pow2(lg(*n / *m)) - 1;

	/* Note a few things we know: 1) both arrays are sorted in ascending order; 
	 * 2) we are looking for nodes to put in the third (output) array. If we 
	 * know that a node at a given location in ARRAY_NODES2 is greater than or
	 * equal to another node in ARRAY_NODES1, we know that all nodes to the 
	 * right of the node in ARRAY_NODES1 are greater than the one we just
	 * compared with from ARRAY_NODES2. As we're filling our third (output)
	 * array from right to left, we can put all nodes to the right of 
	 * ARRAY_NODES1[N + K - 1] in the third (output) array.
	 * If, on the other hand, the node we've compared to is smaller than the
	 * one in ARRAY_NODES2, there's a bit more work to be done..*/
	if (cmp_func(array_nodes1[*n - k - 1].val, array_nodes2[*m - 1].val) >= 0)
	{
		memcpy(array_nodes3 + (next - k) - 1, array_nodes1 + (*n - k) - 1, (k + 1) * sizeof(array_node_t));
		*n -= k + 1;
		next -= k + 1;
	}
	else
	{
		/* We now know that the node at ARRAY_NODES1[N + K - 1] was smaller than
		 * the one we compared it to in ARRAY_NODES2. The one we compared it to
		 * in ARRAY_NODES2 was the last node in that array. As this algorithm
		 * prefers to copy from ARRAY_NODES1 to the third (output) array, we must
		 * find the first node in ARRAY_NODES1 that is greater than or equal to
		 * the last node in ARRAY_NODES2. We can then copy all nodes right of 
		 * that one, including the one itself, to the third (output) array, and 
		 * copy the rightmost node from ARRAY_NODES2 to that array as well. Note
		 * that we could not copy the node from ARRAY_NODES2 safely in the case
		 * it was smaller than the one at N + K - 1 in ARRAY_NODES1, because it
		 * may have been smaller than other nodes, left of N + K - 1, in
		 * ARRAY_NODES1 as well. */
		size_t l = array_binary_search(array_nodes1, *n - k - 1, *n - 1, array_nodes2[*m - 1].val, cmp_func);
		if (l != ~0)
		{
			/* We've found a node in ARRAY_NODES1 greater than or equal to the
			 * last node in ARRAY_NODES2. Anything to the right-hand side of that
			 * node (and the node itself) can be copied to the third (output)
			 * array. */
			memcpy(array_nodes3 + (next - (*n - l)), array_nodes1 + l, (*n - l) * sizeof(array_node_t));
			next -= *n - l;
			*n = l;
		}
		/* As we now now that the node on the right-hand side of ARRAY_NODES2 is
		 * greater than any node left in ARRAY_NODES1, we may as well copy it. */
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
static array_t * array_merge1(array_node_t * array_nodes1, array_node_t * array_nodes2, size_t n, size_t m, libcontain_cmp_func_t cmp_func)
{
	array_t * retval = array_new(n + m);
	size_t next = n + m;

	/* the binary merge implementation changes both N and M, which is why we
	 * have to keep track of them both, and why we handle references to them,
	 * rather than their values. */
	while (n > 0 && m > 0)
	{
		/* The binary merge implementation expects the first array to be greater
		 * in size than the second - at least as far as the nodes left to copy
		 * are concerned. */
		if (n > m)
			next = array_merge2(array_nodes1, array_nodes2, &n, &m, retval->nodes, next, cmp_func);
		else
			next = array_merge2(array_nodes2, array_nodes1, &m, &n, retval->nodes, next, cmp_func);
	}	/* Of course, the reason why this loop exists is that the binary merge
		 * algorithm changes both N and M and still expects N >= M, which this
		 * loop guarantees before each iteration. */
	
	/* If there are any nodes left in the second array, the can all be copied 
	 * to the beginning of third (output) array. If, OTOH, there are no nodes 
	 * left in the second array, there may be nodes left in the first one, in
	 * which case _they_ can be copied to the third (output) array. If there
	 * are no nodes there either, the memcpy function won't do anything. */
	if (n == 0 && m != 0)
		memcpy(retval->nodes, array_nodes2, m * sizeof(array_node_t));
	else
		memcpy(retval->nodes + m, array_nodes1, n * sizeof(array_node_t));

	return retval;
}

array_t * array_merge(array_t * array1, array_t * array2, libcontain_cmp_func_t cmp_func)
{
	array_node_t * nodes1;
	array_node_t * nodes2;
	array_t * retval;
	size_t size1, size2;

	/* get hazardous references to both stretches of nodes. This guarantees that
	 * the memory they point to will not be freed during the process of merging.
	 * It does not guarantee that the contents will not change, and neither does
	 * any other code here. Hence, merge operations should not be combined with
	 * write operations on either one of the arrays.
	 * Locks are ugly and if I can avoid them, I will, but one might be in order
	 * here. */
	do
	{
		nodes1 = array1->nodes;
		hptr_register(0, nodes1);
	} while (nodes1 != array1->nodes);
	size1 = array1->num_entries;
	do
	{
		nodes2 = array2->nodes;
		hptr_register(1, nodes2);
	} while (nodes2 != array2->nodes);
	size2 = array2->num_entries;
	
	retval = array_merge1(nodes1, nodes2, size1, size2, cmp_func);
	/* done with both arrays - release the hazardous references.. */
	hptr_free(0);
	hptr_free(1);

	return retval;
}

/* This function helps qsort figure out which of the two nodes is greater
 * than the other. NULL is always the greatest one, so that full nodes go to
 * the beginning of the array. */
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

	do
	{
		do
		{
			nodes = array->nodes;
			hptr_register(0, nodes);
		} while (nodes != array->nodes);
		/* we no know nodes == array->nodes and nodes will not be freed until were're done */
		qsort(array->nodes, array->size, sizeof(array_node_t), array_condense_helper);
	} while (nodes != array->nodes);
	/* we now know that weve condensed nodes and that those nodes still belonged
	 * to the array when we were done. */
	array->condensed = 1;
	hptr_free(0);
}

/* This is an implementation of a binary merge sort
 * array_nodes is an array of array_node_t's
 * n is the number of nodes in array_nodes
 *
 * This is really a very simple algorithm: just split the work in two until 
 * there's only one or two nodes left, sort the left-over nodes and merge the
 * two results into a single, sorted array. */
static void array_sort_worker(array_node_t * array_nodes, size_t n, libcontain_cmp_func_t cmp_func)
{
	/* split as long as there's more then two nodes left */
	if (n > 2)
	{
		size_t m = n / 2;
		array_t * t_array;

		/* sort the left half */
		array_sort_worker(array_nodes, m + n % 2, cmp_func);
		/* sort the right half */
		array_sort_worker(array_nodes + m + n % 2, m, cmp_func);
		/* merge the two halves */
		t_array = array_merge1(array_nodes, array_nodes + m + n % 2, m + n % 2, m, cmp_func);
		/* copy the result back to where the original was */
		memcpy(array_nodes, t_array->nodes, n * sizeof(array_node_t));
		/* and free the temporary array that resulted from the merge */
		array_free(t_array);
	}
	else if (n == 2)
	{	/* compare the two nodes and exchange if need be */
		if (cmp_func(array_nodes[0].val, array_nodes[1].val) > 0)
		{
			array_node_t t_array_node;

			memcpy(&t_array_node, &(array_nodes[0]), sizeof(array_node_t));
			memcpy(&(array_nodes[0]), &(array_nodes[1]), sizeof(array_node_t));
			memcpy(&(array_nodes[1]), &t_array_node, sizeof(array_node_t));
		}
	}
}

/* This function sorts the array using a binary merge sort. The general idea
 * is to optimally sort bits of the array and then optimally merge the sorted
 * bits. This sort should be optimal - at least in the comparison-based model. 
 * However, I don't have any empirical data to substantiate this other than 
 * a few tests on an old computer.. */
void array_sort(array_t * array, libcontain_cmp_func_t cmp_func)
{
	array_node_t * nodes;
	
	/* We need the array to be condensed in order to sort it. */
	if (array->condensed == 0)
		array_condense(array);
		
	do
	{
		nodes = array->nodes;
		hptr_register(0, nodes);
	} while (nodes != array->nodes);
	/* we now know nodes == array->nodes and nodes will not be freed until we're
	 * done */
	array_sort_worker(nodes, array->num_entries, cmp_func);
	/* Done with the nodes => free the reference */
	hptr_free(0);
	array->sorted = 1;
}

void * array_search(array_t * array, void * val, libcontain_cmp_func_t cmp_func)
{
	size_t rc;
	array_node_t * nodes;
	void * retval = NULL;

	do
	{
		nodes = array->nodes;
		hptr_register(0, nodes);
	} while (nodes != array->nodes);
	/* we now know nodes == array->nodes and nodes will not be freed until we're
	 * done */

	/* if the array is sorted, a binary search is possible and the most optimal
	 * solution */
	if (array->sorted)
		rc = array_binary_search(nodes, 0, array->num_entries - 1, val, cmp_func);
	else	/* otherwise, a linear search is the only way to go */
		rc = array_linear_search(nodes, 0, array->size - 1, val, cmp_func);

	if (rc != ~0)
	{	/* the binary search returns the first node >= the one we're looking for,
		 *but we're looking for the node == VAL. */
		if (cmp_func(val, nodes[rc].val) == 0)
			retval = nodes[rc].val;
	}
	hptr_free(0);

	return retval;
}

/* this function is basically a glorified array_copy combined with a glorified
 * array_foreach: it loops through all of the values in the array and calls the
 * copy function on all the non-NULL ones.. Blindingly obvious code.. */
array_t * array_deep_copy(array_t * array, libcontain_copy_func_t array_valcopy_func)
{
	size_t i;
	size_t size;
	array_t * retval;
	void * val;

	size = array->size;
	retval = array_new(size);
	for (i = 0; i < size; i++)
	{
		val = array_get(array, i);
		if (val != NULL)
			array_put(retval, i, array_valcopy_func(array->nodes[i].val));
	}

	return retval;
}

/* Do the obvious: for each non-NULL value in the array, call the function with
 * the user's data and the node. */
void array_foreach(array_t * array, libcontain_foreach_func_t array_foreach_func, void * data)
{
	int i;
	size_t size;
	void * val;

	size = array->size;

	for (i = 0; i < size; i++)
	{
		val = array_get(array, i);
		if (val != NULL)
			array_foreach_func(val, data);
	}
}
