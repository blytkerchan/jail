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
#ifndef LIBCONTAIN_LIST_H
#define LIBCONTAIN_LIST_H

#include "libcontain_config.h"
#include "types.h"

typedef struct _list_node_t
{
	void * val;
	int mark;
	struct _list_node_t * next;
} list_node_t;

typedef struct _list_t
{
	list_node_t * head;
	libcontain_cmp_func_t cmp_func;
} list_t;

list_t * list_new(libcontain_cmp_func_t cmp_func);
void list_free(list_t * list);
int list_insert(list_t * list, void * val);
int list_delete(list_t * list, void * val);

/* Search a node in the list and return it.
 * This may seem a bit weird, as you hand the node you're looking for as a 
 * parameter to the function, but the reason for this is rather simple: 
 * the compare function need not look at the complete value you've put in
 * the list to decide whether the the value is the one you're looking for.
 * If the value you're looking for is composed of a key and an associated
 * value, the comparison function will only look at the key, whereas it 
 * will be the value you're ultimately interested in. */
void * list_search(list_t * list, void * val);
void list_foreach(list_t * list, libcontain_foreach_func_t helper, void * data);

#endif // LIBCONTAIN_LIST_H
