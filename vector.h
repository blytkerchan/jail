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
#ifndef _LIBCONTAIN_VECTOR_H
#define _LIBCONTAIN_VECTOR_H
/* Vectors are containers that can be indexed by integers, that are easy to 
 * search and iterate, and that rarely change size. 
 * Vectors can easily be sorted, but are not sorted by default.
 * Sorted vectors can easily be searched. In the case of unsorted vectors, that 
 * is somewhat more difficult..
 */
#include "libcontain_config.h"

#include <sys/types.h>
#include "types.h"

#define VECTOR_DEFAULT_SIZE 100
#define VECTOR_DEFAULT_INCREASE 100

typedef struct _vector_node_t
{
	void * val;
} vector_node_t;

typedef struct _vector_t
{
	size_t size;
	size_t num_entries;
	size_t increase;
	vector_node_t * nodes;
	int sorted;
	int condensed;
	int flag;
} vector_t;

/* Create a new, empty vector of size SIZE */
vector_t * vector_new(size_t size);
void free_vector(vector_t * vector);

void * vector_get(vector_t * vector, size_t i);
void vector_put(vector_t * vector, size_t i, void * val);
void vector_push_back(vector_t * vector, void * val);
size_t vector_get_size(vector_t * vector);
size_t vector_get_numentries(vector_t * vector);
void vector_resize(vector_t * vector, size_t size);
vector_t * vector_copy(vector_t * vector);
vector_t * vector_merge(vector_t * vector1, vector_t * vector2, libcontain_cmp_func_t cmp_func);
vector_t * vector_deep_copy(vector_t * vector, libcontain_copy_func_t vector_valcopy_func);
void vector_condense(vector_t * vector);
void vector_sort(vector_t * vector, libcontain_cmp_func_t cmp_func);
void * vector_search(vector_t * vector, void * val, libcontain_cmp_func_t cmp_func);
void vector_set_default_increase(vector_t * vector, size_t increase);
void vector_foreach(vector_t * vector, libcontain_foreach_func_t vector_foreach_func, void * data);

#endif // _LIBCONTAIN_VECTOR_H
