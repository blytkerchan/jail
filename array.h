#ifndef _LIBCONTAIN_ARRAY_H
#define _LIBCONTAIN_ARRAY_H

#include <sys/types.h>
/* Arrays are containers that can be indexed by integers, that are easy to 
 * search and iterate, and that rarely change size. 
 * Arrays can easily be sorted, but are not sorted by default.
 * Sorted arrays can easily be searched. In the case of unsorted arrays, that 
 * is somewhat more difficult..
 */

#define ARRAY_DEFAULT_SIZE 100
#define ARRAY_DEFAULT_INCREASE 100

typedef struct _array_node_t
{
	void * val;
} array_node_t;

typedef struct _array_t
{
	size_t size;
	size_t num_entries;
	size_t increase;
	array_node_t * nodes;
	int sorted;
} array_t;

typedef int (*array_cmp_func_t)(const void * k1, const void * k2);

array_t * new_array(size_t size);
void free_array(array_t * array);

void * array_get(array_t * array, size_t i);
void array_put(array_t * array, size_t i, void * val);
void array_push_back(array_t * array, void * val);
size_t array_get_size(array_t * array);
size_t array_get_numentries(array_t * array);
void array_resize(array_t * array, size_t size);
array_t * array_copy(array_t * array);
array_t * array_merge(array_t * array1, array_t * array2, array_cmp_func_t cmp_func);
void array_condense(array_t * array);
void array_sort(array_t * array, array_cmp_func_t cmp_func);
void * array_search(array_t * array, void * val, array_cmp_func_t cmp_func);
void array_set_default_increase(array_t * array, size_t increase);

#endif // _LIBCONTAIN_ARRAY_H
