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
#include "libmemory/smr.h"
#include "../array.h"
#include "test.h"

int array_test1_sort_helper(const void * ptr1, const void * ptr2)
{
	return ptr1 - ptr2;
}

void array_test1(void)
{	/* test the sorting */
	int i, prev = 0, new;
	array_t * array = new_array(0);
	
	for (i = 0; i < 2 * ARRAY_DEFAULT_SIZE; i++)
		array_push_back(array, (void*)rnd(1,100));
	for (i = 0; i < 2 * ARRAY_DEFAULT_SIZE; i++)
		if (((int)array_get(array, i)) == 0)
			exit(1);
	array_sort(array, array_test1_sort_helper);
	for (i = 0; i < 2 * ARRAY_DEFAULT_SIZE; i++)
	{
		new = (int)array_get(array, i);
		if (new < prev)
			exit(1);
		prev = new;
	}

	free_array(array);
}

struct array_test2_t
{
	int index;
	char val[21];
};

int array_test2_compare(const void * ptr1, const void * ptr2)
{
	struct array_test2_t * t1 = (struct array_test2_t *)ptr1;
	struct array_test2_t * t2 = (struct array_test2_t *)ptr2;

	return t1->index - t2->index;
}

void array_test2(void)
{	/* test the searching */
	int i, j;
	array_t * array = new_array(0);
	struct array_test2_t * testval;
	struct array_test2_t * searchval;

	for (i = 0; i < 20; i++)
	{
		testval = (struct array_test2_t*)malloc(sizeof(struct array_test2_t));
		testval->index = rnd(1, 100);
		for (j = 0; j < 20; j++)
			testval->val[j] = rnd('a', 'z');
		testval->val[20] = 0;

		array_put(array, i, testval);
		if (i == 10)
			searchval = testval;
	}
	testval = array_search(array, searchval, array_test2_compare);
	if (strcmp(testval->val, searchval->val) != 0)
		exit(1);
	array_sort(array, array_test2_compare);
	testval = array_search(array, searchval, array_test2_compare);
	if (strcmp(testval->val, searchval->val) != 0)
		exit(1);

	free_array(array);
}

void array_test3(void)
{	/* test out-of-bounds putting */
	void * tt = "hello";
	
	array_t * array = new_array(0);
	array_put(array,   1, tt);
	array_put(array, 101, tt);
	array_put(array, 501, tt);
	
	free_array(array);
}

int main(void)
{
	smr_init(LIBCONTAIN_MIN_HPTRS);
	hptr_init();
	smr_thread_init();
	
	array_test1();
	array_test2();
	array_test3();

	smr_thread_fini();
	hptr_fini();
	smr_fini();

	return 0;
}

