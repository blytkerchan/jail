/* $Id: Hash.cc,v 1.3 2004/04/01 13:35:10 blytkerchan Exp $ */
/* Jail: Just Another Interpreted Language
 * Copyright (c) 2003, Ronald Landheer-Cieslak
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
#else  // in native windows
#define HAVE_STRING_H 1
#endif
#include <assert.h>
#include "Hash.h"
#include <stdlib.h>

#ifdef HAVE_STRING_H
	#include <string.h>
#else 	// !HAVE_STRING
	#ifdef HAVE_STRINGS_H
		#include <strings.h>
	#else // !HAVE_STRINGS
		#error Neither string.h nor strings.h is available
		#error Either file is required for the operation of this library
		#error Please contact ronald@landheer.com for more information
	#endif // !HAVE_STRINGS
#endif 	// !HAVE_STRING

/***
// Constants */
static const char *HASH_MAGIC = "HASH";
static const char *HASH_VERSION = "0100";

/*** 
// Local functions */
/** 
// get_prime(): get a prime greater than or equal to _U, return ~0 on error (_U > 3355070839)
*/
ulong get_prime(ulong _u) {
	static const ulong primes[] = {
		19, 29, 41, 59, 79, 107, 149, 197, 263, 347, 457, 599, 787, 1031,
		1361, 1777, 2333, 3037, 3967, 5167, 6719, 8737, 11369, 14783,
		19219, 24989, 32491, 42257, 54941, 71429, 92861, 120721, 156941,
		204047, 265271, 344857, 448321, 582821, 757693, 985003, 1280519,
		1664681, 2164111, 2813353, 3657361, 4754591, 6180989, 8035301,
		10445899, 13579681, 17653589, 22949669, 29834603, 38784989,
		50420551, 65546729, 85210757, 110774011, 144006217, 187208107,
		243370577, 316381771, 411296309, 534685237, 695090819, 903618083,
		1174703521, 1527114613, 1985248999, 2580823717UL, 3355070839UL
	};
	size_t i;
	
	for (i = 0; i < (sizeof(primes) / sizeof(ulong)); i++) {
		if (primes[i] >= _u) return (primes[i]);
	} // for

	return (~0);
} // get_prime()

/*** 
// Constructors */
/** 
// Hash(void): construct a default empty hash
*/
Hash::Hash(void) {
	Count = 0;
	size = get_prime(Count); // FIXME: check this for errors (1)
	mappings = (mapping_type*)malloc(size * sizeof(mapping_type));
	memset(mappings, 0, size * sizeof(mapping_type));
} // Hash()

/** 
// Hash(ulong): construct a default empty hash with initial size C
// NOTE: this is not tested by the test1 test program
*/
Hash::Hash(ulong C) {
	Count = 0;
	size = get_prime(C); // FIXME: check this for errors (1)
	mappings = (mapping_type*)malloc(size * sizeof(mapping_type));
	memset(mappings, 0, size * sizeof(mapping_type));
} // Hash()

/*** 
// Destructors */
/** 
// ~Hash(): free everything allocated
*/
Hash::~Hash(void) {
	if (mappings) free(mappings);
	mappings = NULL;
} // ~Hash()

/*** 
// Public class functions */
/** 
// get(): gets the value corresponding to a key, returns NULL if it doesn't exist
*/
void *Hash::get(const void *key) {
	return(_get(key));
} // get()

void *Hash::_get(const void *key) {
	int location = hash(key) % size;
	ulong i;
	
	for (i = location; (i < size) && !empty_key(mappings[i].key); i++) {
		if (cmp_keys(key, mappings[i].key) == 0) return(mappings[i].value);
	} // for
		
	return (NULL);
} // _get()

/** 
// put(): puts the value VALUE in the hash at key KEY
//        returns true if successful, false if not
*/
bool Hash::put(const void *key, const void *value) {
	return(_put(key, value));
} // put

bool Hash::_put(const void *key, const void *value) {
	int location = hash(key) % size;
	ulong i;
	
	if (((double)(Count + 1) / (double)size) >= 0.75) { // FIXME: make the 0.75 a define - perhaps in config.h?
		if (!grow(size + 1)) return (false);
		return(_put(key, value));
	} // if
	for (i = location; true; i++) {
		if (i == size) {
			if (grow(size + 1)) 
				return (_put(key, value));
			else return (false);
		} // if
		if (empty_key(mappings[i].key)) Count++;
		if (empty_key(mappings[i].key) || (cmp_keys(mappings[i].key, key) == 0)) {
			mappings[i].key = (void*)key;
			mappings[i].value = (void*)value;
			return (true);
		} // if
	} // for

	return(false); // never reached (normally)
} // _put()

/** 
// remove(): remove an entry from the hash
//           we rehash when we've removed the entry to make sure all 
//           entries end up in the right place
// FIXME: this could probably be done more efficiently
*/
bool Hash::remove(const void *key) {
	return(_remove(key));
} // remove()

bool Hash::_remove(const void *key) {
	mapping_type *old_mappings = mappings;
	int location = hash(key) % size;
	ulong i;
	
	for (i = location; (i < size) && !empty_key(old_mappings[i].key); i++) {
		if (cmp_keys(key, old_mappings[i].key) == 0) {
			old_mappings[i].key = NULL;
			mappings = (mapping_type*)malloc(size * sizeof(mapping_type));
			memset(mappings, 0, size * sizeof(mapping_type));
			Count = 0;
			for (i = 0; i < size; i++) {
				if (!empty_key(old_mappings[i].key)) {
					_put(old_mappings[i].key, old_mappings[i].value);
				} // if
			} // for
			free(old_mappings);
			return (true);
		} // if
	} // for
		
	return (false);
} // remove()

/** 
// clear(): clear the hash of all its contents
*/
bool Hash::clear(void) {
	memset(mappings, 0, size * sizeof(mapping_type));
	Count = 0;
		
	return (true);
} // clear()

/** 
// count(): count the number of entries in the hash
*/
ulong Hash::count(void) {
	return (Count);
} // count()

/** 
// keys(): return a list of all keys in the hash
//         returns NULL on error or if there are no keys, otherwise, a pointer to a static array ending with a NULL
// NOTE: this could be done more efficiently (for time) if we cached the list..?
*/
void **Hash::keys(void) {
	return(_keys());
} // keys

void **Hash::_keys(void) {
	static void **keys = NULL;
	unsigned int num_keys = 0;
	ulong i;
	
	if ((keys = (void**)malloc((this->Count + 1) * sizeof(void*))) == NULL) return (NULL);
	for (i = 0; i < size; i++) {
		if (!empty_key(mappings[i].key)) {
			keys[num_keys] = mappings[i].key;
			keys[num_keys + 1] = NULL;
			num_keys++;
		} // if
	} // for
	assert(Count == num_keys);
	
	return(keys);
} // keys()

/** 
// contains(): return true if the key exists, false if not
// NOTE: this could be done more efficiently (for time) if we cashed the list..?
*/
bool Hash::contains(const void *key) {
	return(_contains(key));
} // contains

bool Hash::_contains(const void *key) {
	int location = hash(key) % size;
	ulong i;
	
	for (i = location; i < size && !empty_key(mappings[i].key); i++) {
		if (cmp_keys(key, mappings[i].key) == 0) return true;
	} // for
	
	return false;
} // _contains()

/** 
// cmp_keys(): compare two keys, return 0 if they're the same, nonzero if not
*/
int Hash::cmp_keys(const void *key1, const void *key2) {
	key_type *K1 = (key_type*)key1, *K2 = (key_type*)key2;

	if (K1 == K2) return (0);
	if ((K1 == NULL) || (K2 == NULL)) return (-1);
	if (K1->size != K2->size)
		return (K2->size - K1->size);
	return (memcmp(K1->value, K2->value, K1->size));
} // cmp_keys()

/** 
// hash(): calculates a 32-bit Adler CRC for *VALUE, used to hash the value.
*/
ulong Hash::hash(const void *key) {
	key_type *K = (key_type*)key;
	char *value = (char*)K->value;
	ulong currentAdler1 = 0x00000000UL;
    	ulong currentAdler2 = 0x00000001UL;
    	int i;
    	
    	for (i = 0; i < K->size; i++) {
    		currentAdler1 += value[i];
    		currentAdler2 += currentAdler1;
    	} // for
    	currentAdler1 = currentAdler1 << 16;
    	
    	return (currentAdler1 + currentAdler2);
} // hash()

/** 
// grow(): grow the hash to S (minimum)
*/
bool Hash::grow(int S) {
	mapping_type *old_mappings = mappings;
	ulong i, old_size;

	old_size = size;
	S = get_prime(S);
	if ((mappings = (mapping_type*)malloc(S * sizeof(mapping_type))) == NULL) return (false);
	memset(mappings, 0, S * sizeof(mapping_type));
	Count = 0;
	size = S;
	for (i = 0; i < old_size; i++) {
		if (!empty_key(old_mappings[i].key)) {
			_put(old_mappings[i].key, old_mappings[i].value);
		} // if
	} // for
	free(old_mappings);
	
	return(true);
} // grow()

/** 
// empty_key(): check whether a key is empty - return true if so, false if not
*/
bool Hash::empty_key(void* key) {
	key_type *K = (key_type *)key;
	
	if (K == NULL) return (true);
	return(K->size == 0);
} // empty_key()

bool Hash::read(char *filename) {
	strcpy(this->filename, filename);
	FILE *file;
	int data_size;
	static key_type *key;
	char magic[4];
	void *data = NULL;
	
	assert(sizeof(int) == 4);
	if ((file = fopen(filename, "rb")) == NULL) return(false);
	// check the magic number
	fread(magic, 4, 1, file);
	if (memcmp(magic, HASH_MAGIC, 4) != 0) return(false);
	fread(magic, 4, 1, file);
	if (memcmp(magic, HASH_VERSION, 4) != 0) return(false);
	while(fread(&data_size, 4, 1, file) == 1) {
		key = (key_type*)malloc(sizeof(key_type));
		key->value = NULL; // just to make sure
		key->size = data_size;
		key->value = malloc(key->size);
		fread(key->value, key->size, 1, file);
		fread(&data_size, 4, 1, file);
		data = malloc(data_size);
		fread(data, data_size, 1, file);
		this->_put(key, data);
	} // while
	fclose(file);
	
	return(true);
} // read()

bool Hash::write(char *filename, int (*helper_function)(void*)) {
	strcpy(this->filename, filename);
	write_helper_function = helper_function;
	return(this->write());
} // write()

bool Hash::write(int (*helper_function)(void*)) {
	write_helper_function = helper_function;
	return(this->write());
} // write()

bool Hash::write(char *filename) {
	strcpy(this->filename, filename);
	return(this->write());
} // write()

bool Hash::write(void) {
	return(_write());
} // write

bool Hash::_write(void) {
	key_type **key_list;
	int i, data_size;
	FILE *file;
	void *data;
	
	assert(sizeof(int) == 4);
	if ((file = fopen(filename, "wb")) == NULL) return(false);
	fwrite(HASH_MAGIC, 4, 1, file);
	fwrite(HASH_VERSION, 4, 1, file);
	key_list = (key_type**)this->keys();
	for (i = 0; key_list[i]; i++) {
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

void Hash::set_write_helper_function(int (*helper_function)(void*)) {
	write_helper_function = helper_function;
} // set_write_helper_function()

void * Hash::get_write_helper_function(void) {
	return((void*)write_helper_function);
} // set_write_helper_function()

char *Hash::get_filename(void) {
	return(filename);
} // get_filename()

void Hash::set_filename(char *filename) {
	strcpy(this->filename, filename);
} // get_filename()

// rlc	2004-03-31
// added to implement hash_search
void Hash::for_each(foreach_func_t func, void * user_data)
{
	ulong i;
	
	for (i = 0; i < Count; i++)
	{
		if (mappings[i].key)
			func(mappings[i].key, mappings[i].val, user_data);
	}
}

/* FOOTNOTES
// 1. Constructors do not return any value, so we should get back to the good old RunControl variable..
*/

