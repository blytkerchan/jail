/* $Id: Hash.cc,v 1.7 2004/07/21 19:51:41 blytkerchan Exp $ */
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
#include <cassert>
#include "Hash.h"
#include <cstdlib>
#include <cstring>

#include "arch/include/increment.h"
#include "arch/include/decrement.h"
#include "prime.h"
#include "thread.h"

/***
// Constants */
static const char *HASH_MAGIC = "HASH";
static const char *HASH_VERSION = "0100";

/***
// Constructors */
/**
// Hash(void): construct a default empty hash
*/
Hash::Hash(void)
{
	Count = 0;
	size = get_prime(Count);
	mappings = (mapping_type*)calloc(size, sizeof(mapping_type));
}

/**
// Hash(unsigned int): construct a default empty hash with initial size C
// NOTE: this is not tested by the test1 test program
*/
Hash::Hash(unsigned int C)
{
	Count = 0;
	size = get_prime(C);
	mappings = (mapping_type*)calloc(size, sizeof(mapping_type));
}

/***
// Destructors */
/**
// ~Hash(): free everything allocated
*/
Hash::~Hash(void)
{
	if (mappings)
		free(mappings);
	mappings = NULL;
} // ~Hash()

/***
// Public class functions */
/**
// get(): gets the value corresponding to a key, returns NULL if it doesn't exist
*/
void *Hash::get(const void *key)
{
	void * retval;

	reg_reader();
	retval = _get(key);
	ureg_reader();
	
	return retval;
} // get()

void *Hash::_get(const void *key)
{
	mapping_type * bucket;
	void * retval = NULL;

	bucket = find(key);
	if (bucket)
	{
		retval = bucket->value;
		ureg_breader(bucket);
	}
	
	return retval;
}

/**
// put(): puts the value VALUE in the hash at key KEY
//        returns true if successful, false if not
*/
bool Hash::put(const void *key, const void *value)
{
	bool retval;

	reg_reader();
	retval = _put(key, value);
	ureg_reader();

	return retval;
} // put

bool Hash::_put(const void *key, const void *value)
{
	mapping_type * bucket;

	while (1)
	{
		bucket = find(key, true);
		if (bucket == NULL)
		{
			grow(size + 1);
			continue;
		}
		if (empty_key(bucket->key) || !bucket->value)
		{
			atomic_increment((uint32_t*)&Count);
			if (((Count * 4) / 3) > size)
			{
				bunlock(bucket);
				ureg_breader(bucket);
				grow((Count * 4) / 3);
				atomic_decrement((uint32_t*)&Count);
				continue;
			}
		}
		bucket->key = const_cast<void*>(key);
		bucket->value = const_cast<void*>(value);
		bucket->hash = hash(key);
		bunlock(bucket);
		ureg_breader(bucket);
		break;
	}

	return true;
} // _put()

/**
// remove(): remove an entry from the hash
*/
bool Hash::remove(const void *key)
{
	bool retval;

	reg_reader();
	retval = _remove(key);
	ureg_reader();
	
	return retval;
} // remove()

bool Hash::_remove(const void *key)
{
	mapping_type * bucket;

	bucket = find(key, true);
	if (bucket == NULL)
		return false;
	bucket->value = NULL;
	bunlock(bucket);
	ureg_breader(bucket);

	return true;
} // remove()

/**
// clear(): clear the hash of all its contents
*/
bool Hash::clear(void)
{
	lock();
	memset(mappings, 0, size * sizeof(mapping_type));
	Count = 0;
	unlock();

	return true;
} // clear()

/**
// count(): count the number of entries in the hash
*/
unsigned int Hash::count(void)
{
	unsigned int retval;

	reg_reader();
	retval = Count;
	ureg_reader();
	
	return retval;
} // count()

/**
// keys(): return a list of all keys in the hash
//         returns NULL on error or if there are no keys, otherwise, a pointer to an array ending with a NULL
//	the returned memory needs to be freed by the caller
*/
void **Hash::keys(void)
{
	void ** retval;
	
	reg_reader();
	retval = _keys();
	ureg_reader();

	return retval;
} // keys

void **Hash::_keys(void)
{
	void **keys = NULL;
	unsigned int num_keys = 0;
	unsigned int i;

	// Count might change but size won't
	if ((keys = (void**)malloc((size + 1) * sizeof(void*))) == NULL)
		return (NULL);
	for (i = 0; i < size; i++)
	{
		reg_breader(&(mappings[i]));
		if (!empty_key(mappings[i].key) && mappings[i].value)
		{
			keys[num_keys] = mappings[i].key;
			keys[num_keys + 1] = NULL;
			num_keys++;
		} // if
		ureg_breader(&(mappings[i]));
	} // for

	return(keys);
} // keys()

/**
// contains(): return true if the key exists, false if not
// NOTE: this could be done more efficiently (for time) if we cashed the list..?
*/
bool Hash::contains(const void *key)
{
	bool retval;
	
	reg_reader();
	retval = _contains(key);
	ureg_reader();
	
	return retval;
} // contains

bool Hash::_contains(const void *key)
{
	bool retval = false;
	mapping_type * bucket;

	bucket = find(key);
	if (bucket != NULL)
	{
		retval = (bucket->value != NULL);
		ureg_breader(bucket);
	}
	
	return retval;
} // _contains()

/**
// cmp_keys(): compare two keys, return 0 if they're the same, nonzero if not
*/
int Hash::cmp_keys(const void *key1, const void *key2)
{
	key_type *K1 = (key_type*)key1, *K2 = (key_type*)key2;

	if (K1 == K2)
		return (0);
	if ((K1 == NULL) || (K2 == NULL))
		return (-1);
	if (K1->size != K2->size)
		return (K2->size - K1->size);
	return (memcmp(K1->value, K2->value, K1->size));
} // cmp_keys()

/**
// hash(): calculates a 32-bit Adler CRC for *VALUE, used to hash the value.
// Note that this is not quite an Adler CRC, as the Adler CRC modulizes bith ends by a constant, which we don't do.
*/
unsigned int Hash::hash(const void *key)
{
	key_type *K = (key_type*)key;
	char *value = (char*)K->value;
	unsigned int currentAdler1 = 1;
	unsigned int currentAdler2 = 0;
	int i;

	for (i = 0; i < K->size; i++)
	{
		currentAdler1 += value[i];
		currentAdler2 += currentAdler1;
	} // for
	currentAdler1 = currentAdler1 << 16;

	return (currentAdler1 + currentAdler2);
} // hash()

/**
// grow(): grow the hash to S (minimum)
*/
bool Hash::grow(unsigned int S)
{
	mapping_type * old_mappings;
	mapping_type * bucket;
	unsigned int i, old_size;

	lock(1);
	old_mappings = mappings;
	old_size = size;
	S = get_prime(S);
	if (S == size)
	{
		unlock();
		return true;
	}
	if ((mappings = (mapping_type*)calloc(S, sizeof(mapping_type))) == NULL)
	{
		mappings = old_mappings;
		unlock();
		return false;
	}
	Count = 0;
	size = S;
	for (i = 0; i < old_size; i++)
	{
		bucket = select(old_mappings[i].hash % size, old_mappings[i].key, true);
		bucket->key = old_mappings[i].key;
		bucket->value = old_mappings[i].value;
		bucket->hash = old_mappings[i].hash;
		bunlock(bucket);
		ureg_breader(bucket);
	} // for
	free(old_mappings);
	unlock();

	return(true);
} // grow()

/**
// empty_key(): check whether a key is empty - return true if so, false if not
*/
bool Hash::empty_key(void* key)
{
	key_type *K = (key_type *)key;

	if (K == NULL)
		return (true);
	return(K->size == 0);
} // empty_key()

/* deprecated */
bool Hash::read(char *filename)
{
	strcpy(this->filename, filename);
	FILE *file;
	int data_size;
	static key_type *key;
	char magic[4];
	void *data = NULL;

	reg_reader();
	assert(sizeof(int) == 4);
	if ((file = fopen(filename, "rb")) == NULL)
	{
		ureg_reader();
		return(false);
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
	while (fread(&data_size, 4, 1, file) == 1)
	{
		key = (key_type*)malloc(sizeof(key_type));
		key->size = data_size;
		key->value = malloc(key->size);
		fread(key->value, key->size, 1, file);

		fread(&data_size, 4, 1, file);
		data = malloc(data_size);
		fread(data, data_size, 1, file);
		this->_put(key, data);
	} // while
	fclose(file);

	ureg_reader();
	return(true);
} // read()

/* deprecated */
bool Hash::write(char *filename, int (*helper_function)(void*))
{
	bool retval;
	strcpy(this->filename, filename);
	write_helper_function = helper_function;

	reg_reader();
	retval = _write();
	ureg_reader();

	return retval;
} // write()

/* deprecated */
bool Hash::write(int (*helper_function)(void*))
{
	bool retval;
	write_helper_function = helper_function;

	reg_reader();
	retval = _write();
	ureg_reader();

	return retval;
} // write()

/* deprecated */
bool Hash::write(char *filename)
{
	bool retval;
	
	strcpy(this->filename, filename);
	reg_reader();
	retval = _write();
	ureg_reader();

	return retval; 
} // write()

/* deprecated */
bool Hash::write(void)
{
	bool retval;
	
	reg_reader();
	retval = _write();
	ureg_reader();

	return retval; 
} // write

/* deprecated */
bool Hash::_write(void)
{
	key_type **key_list;
	int i, data_size;
	FILE *file;
	void *data;

	assert(sizeof(int) == 4);
	if ((file = fopen(filename, "wb")) == NULL)
		return(false);
	fwrite(HASH_MAGIC, 4, 1, file);
	fwrite(HASH_VERSION, 4, 1, file);
	key_list = (key_type**)this->keys();
	for (i = 0; key_list[i]; i++)
	{
		fwrite(&key_list[i]->size, 4, 1, file);
		fwrite(key_list[i]->value, key_list[i]->size, 1, file);
		data = this->_get(key_list[i]);
		data_size = write_helper_function(data);
		fwrite(&data_size, 4, 1, file);
		fwrite(data, data_size, 1, file);
	} // for
	fclose(file);

	return(true);
} // write()

/* deprecated */
void Hash::set_write_helper_function(write_helper_func_t func)
{
	write_helper_function = func;
} // set_write_helper_function()

/* deprecated */
Hash::write_helper_func_t Hash::get_write_helper_function(void)
{
	return(write_helper_function);
} // set_write_helper_function()

/* deprecated */
char *Hash::get_filename(void)
{
	return(filename);
} // get_filename()

/* deprecated */
void Hash::set_filename(char *filename)
{
	strcpy(this->filename, filename);
} // get_filename()

// rlc	2004-03-31
// added to implement hash_search
void Hash::for_each(foreach_func_t func, void * user_data)
{
	unsigned int i;
	
	reg_reader();
	for (i = 0; i < Count; i++)
	{
		reg_breader(&(mappings[i]));
		if (!empty_key(mappings[i].key))
		{
			if (mappings[i].value)
				func(mappings[i].key, mappings[i].value, user_data);
		}
		
	}
	ureg_reader();
}

void Hash::reg_reader(void)
{
	atomic_increment((uint32_t*)&readers);
	while (_lock)
	{
		atomic_decrement((uint32_t*)&readers);
		thread_interrupt();
		atomic_increment((uint32_t*)&readers);
	}
}

void Hash::ureg_reader(void)
{
	atomic_decrement((uint32_t*)&readers);
}

void Hash::reg_breader(mapping_type * bucket)
{
	atomic_increment((uint32_t*)&(bucket->readers));
	while (bucket->lock)
	{
		atomic_decrement((uint32_t*)&(bucket->readers));
		thread_interrupt();
		atomic_increment((uint32_t*)&(bucket->readers));
	}
}

void Hash::ureg_breader(mapping_type * bucket)
{
	atomic_decrement((uint32_t*)&(bucket->readers));
}

void Hash::lock(unsigned int n_readers)
{
	atomic_increment((uint32_t*)&_lock);
	while ((_lock != 1) || (readers != n_readers))
	{
		atomic_decrement((uint32_t*)&_lock);
		thread_interrupt();
		atomic_increment((uint32_t*)&_lock);
	}
}

void Hash::unlock(void)
{
	atomic_decrement((uint32_t*)&_lock);
}

void Hash::block(mapping_type * bucket)
{
	atomic_increment((uint32_t*)&(bucket->lock));
	while ((bucket->lock != 1) || (bucket->readers != 1))
	{
		atomic_decrement((uint32_t*)&(bucket->readers));
		atomic_decrement((uint32_t*)&(bucket->lock));
		thread_interrupt();
		atomic_increment((uint32_t*)&(bucket->lock));
		atomic_increment((uint32_t*)&(bucket->readers));
	}
}

void Hash::bunlock(mapping_type * bucket)
{
	atomic_decrement((uint32_t*)&(bucket->lock));
} 

mapping_type * Hash::select(unsigned int index, const void * key, bool for_write)
{
	unsigned int i;
	bool started = false;
	
	for (i = index; (i != index) || !started; i = (i + 1) % size)
	{
		started = true;
		reg_breader(&(mappings[i]));
		if (empty_key(mappings[i].key))
		{
			if (for_write)
			{
				block(&(mappings[i]));
				return &(mappings[i]);
			}
			else
			{
				ureg_breader(&(mappings[i]));
				return NULL;
			}
		}
		if (cmp_keys(mappings[i].key, key) == 0)
		{
			if (for_write)
				block(&(mappings[i]));
			return &(mappings[i]);
		}
		ureg_breader(&(mappings[i]));
	}

	return NULL;
}

mapping_type * Hash::find(const void * key, bool for_write)
{
	unsigned int index;

	index = hash(key) % size;
	return select(index, key, for_write);
}

