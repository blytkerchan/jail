/* $Id: IntHash.cc,v 1.4 2004/05/07 16:28:21 blytkerchan Exp $ */
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
/***
// Includes */
#if !defined(_WIN32) || defined (__CYGWIN__)
#include <config.h>
#endif // not in native windows
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "Hash.h"
#include "IntHash.h"

/***
// Constants */
static const char *HASH_MAGIC = "HASI";
static const char *HASH_VERSION = "0100";

/***
// Constructors */
IntHash::IntHash() : Hash()
{}

IntHash::IntHash(unsigned int _u) : Hash(_u)
{}

/***
// Class functions */
bool IntHash::empty_key(void * key)
{
	int K = (int)key;

	return(K == 0);
} // empty_key()

/**
// cmp_keys(): compare two keys, return 0 if they're the same, nonzero if not
*/
int IntHash::cmp_keys(const void *key1, const void *key2)
{
	return ((int)key2 - (int)key1);
} // cmp_keys()

/* Algorithm and code taken directly from wget - simply messes up the integer.. */
unsigned int IntHash::hash(const void *key)
{
	unsigned int K = (int)key;

	K += (K << 12);
	K ^= (K >> 22);
	K += (K << 4);
	K ^= (K >> 9);
	K += (K << 10);
	K ^= (K >> 2);
	K += (K << 7);
	K ^= (K >> 12);

	return ((unsigned int)K);
} // hash()

bool IntHash::read(char *filename)
{
	strcpy(this->filename, filename);
	FILE *file;
	int data_size;
	int key;
	char magic[4];
	void *data = NULL;

	reg_reader();

	assert(sizeof(int) == 4);
	if ((file = fopen(filename, "rb")) == NULL)
	{
		ureg_reader();
		return (false);
	}
	// check the magic number
	fread(magic, 4, 1, file);
	if (memcmp(magic, HASH_MAGIC, 4) != 0)
	{
		ureg_reader();
		return(false);
	}
	fread(magic, 4, 1, file);
	if (memcmp(magic, HASH_VERSION, 4) != 0)
	{
		ureg_reader();
		return(false);
	}
	while(fread(&key, 4, 1, file) == 1)
	{
		fread(&data_size, 4, 1, file);
		data = malloc(data_size);
		fread(data, data_size, 1, file);
		this->_put((void*)(++key), data);
	} // while
	fclose(file);

	ureg_reader();
	return(true);
} // read()

bool IntHash::_write(void)
{
	int *key_list;
	int i, data_size;
	FILE *file;
	void *data;

	assert(sizeof(int) == 4);
	if ((file = fopen(filename, "wb")) == NULL)
		return(false);
	fwrite(HASH_MAGIC, 4, 1, file);
	fwrite(HASH_VERSION, 4, 1, file);
	key_list = (int*)this->keys();
	for (i = 0; key_list[i] != -1; i++)
	{
		fwrite(&key_list[i], 4, 1, file);
		data = this->get
		       (key_list[i]);
		data_size = write_helper_function(data);
		fwrite(&data_size, 4, 1, file);
		fwrite(data, data_size, 1, file);
	} // for
	fclose(file);

	return(true);
} // write()

// we have reserved actual key value 0, but as the constructor initialises everything to 0, we will store
// every key incremented by one, and not tell the user about it
void *IntHash::get
	(const int key)
{
	void * retval;
	
	int L = key + 1;
	reg_reader();
	retval = _get((void*)L);
	ureg_reader();
	
	return retval;
} // get()

bool IntHash::put(const int key, const void *value)
{
	bool retval;
	int L = key + 1;

	reg_reader();
	retval = _put((void*)L, value);
	ureg_reader();
	
	return retval;
} // put

bool IntHash::contains(const int key)
{
	bool retval;
	int L = key + 1;

	reg_reader();
	retval = _contains((void*)L);
	ureg_reader();

	return retval;
} // contains

void **IntHash::keys(void)
{
	int i;
	int *key_list;

	reg_reader();
	key_list = (int*)_keys();
	ureg_reader();
	
	for (i = 0; ; i++)
	{
		key_list[i] -= 1;
		if (key_list[i] == -1)
			break;
	} // for

	return((void**)key_list);
} // keys()

bool IntHash::remove
	(const int key)
{
	bool retval;
	int L = key + 1;
	
	reg_reader();
	retval = _remove((void*)L);
	ureg_reader();

	return retval;
} // remove()

