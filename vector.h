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
#ifndef _LIBCONTAIN_ARRAY_H
#define _LIBCONTAIN_ARRAY_H
/* Arrays are containers that can be indexed by integers, that are easy to 
 * search and iterate, and that rarely change size. 
 * Arrays can easily be sorted, but are not sorted by default.
 * Sorted arrays can easily be searched. In the case of unsorted arrays, that 
 * is somewhat more difficult..
 */
#include "libcontain_config.h"

#include <sys/types.h>
#include "types.h"

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
	int condensed;
	int flag;
} array_t;

/* Create a new, empty array of size SIZE */
array_t * array_new(size_t size);
void free_array(array_t * array);

void * array_get(array_t * array, size_t i);
void array_put(array_t * array, size_t i, void * val);
void array_push_back(array_t * array, void * val);
size_t array_get_size(array_t * array);
size_t array_get_numentries(array_t * array);
void array_resize(array_t * array, size_t size);
array_t * array_copy(array_t * array);
array_t * array_merge(array_t * array1, array_t * array2, libcontain_cmp_func_t cmp_func);
array_t * array_deep_copy(array_t * array, libcontain_copy_func_t array_valcopy_func);
void array_condense(array_t * array);
void array_sort(array_t * array, libcontain_cmp_func_t cmp_func);
void * array_search(array_t * array, void * val, libcontain_cmp_func_t cmp_func);
void array_set_default_increase(array_t * array, size_t increase);
void array_foreach(array_t * array, libcontain_foreach_func_t array_foreach_func, void * data);

#endif // _LIBCONTAIN_ARRAY_H
