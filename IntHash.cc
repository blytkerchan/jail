/* $Id: IntHash.cc,v 1.1 2003/10/07 16:42:39 blytkerchan Exp $ */
/* Formatted log output
   Copyright (C) 2002
   Ronald Landheer <info@rlsystems.net>

This file is part of the Real Life Systems Hash Library (libhash).

libhash is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

libhash is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with libhash; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */
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
IntHash::IntHash() : Hash() {
} // IntHash()

IntHash::IntHash(ulong _u) : Hash(_u) {
} // IntHash()

/*** 
// Class functions */
bool IntHash::empty_key(void *key) {
	int K = (int)key;
	
	return(K == 0);
} // empty_key()

/** 
// cmp_keys(): compare two keys, return 0 if they're the same, nonzero if not
*/
int IntHash::cmp_keys(const void *key1, const void *key2) {
	return ((int)key2 - (int)key1);
} // cmp_keys()

/* Algorithm and code taken directly from wget - simply messes up the integer.. */
ulong IntHash::hash(const void *key) {
	ulong K = (int)key;
	
	K += (K << 12);
	K ^= (K >> 22);
	K += (K << 4);
	K ^= (K >> 9);
	K += (K << 10);
	K ^= (K >> 2);
	K += (K << 7);
	K ^= (K >> 12);

	return ((ulong)K);
} // hash()

bool IntHash::read(char *filename) {
	strcpy(this->filename, filename);
	FILE *file;
	int data_size;
	int key;
	char magic[4];
	void *data = NULL;
	
	assert(sizeof(int) == 4);
	if ((file = fopen(filename, "rb")) == NULL) return(false);
	// check the magic number
	fread(magic, 4, 1, file);
	if (memcmp(magic, HASH_MAGIC, 4) != 0) return(false);
	fread(magic, 4, 1, file);
	if (memcmp(magic, HASH_VERSION, 4) != 0) return(false);
	while(fread(&key, 4, 1, file) == 1) {
		fread(&data_size, 4, 1, file);
		data = malloc(data_size);
		fread(data, data_size, 1, file);
		this->put(key, data);
	} // while
	fclose(file);
	
	return(true);
} // read()

bool IntHash::_write(void) {
	int *key_list;
	int i, data_size;
	FILE *file;
	void *data;
	
	assert(sizeof(int) == 4);
	if ((file = fopen(filename, "wb")) == NULL) return(false);
	fwrite(HASH_MAGIC, 4, 1, file);
	fwrite(HASH_VERSION, 4, 1, file);
	key_list = (int*)this->keys();
	for (i = 0; key_list[i] != -1; i++) {
		fwrite(&key_list[i], 4, 1, file);
		data = this->get(key_list[i]);
		data_size = write_helper_function(data);
		fwrite(&data_size, 4, 1, file);
		fwrite(data, data_size, 1, file);
	} // for
	fclose(file);
	
	return(true);
} // write()

// we have reserved actual key value 0, but as the constructor initialises everything to 0, we will store 
// every key incremented by one, and not tell the user about it
void *IntHash::get(const int key) {
	int L = key + 1;
	return(_get((void *)L));
} // get()

bool IntHash::put(const int key, const void *value) {
	int L = key + 1;
	return(_put((void*)L, value));
} // put

bool IntHash::contains(const int key) {
	int L = key + 1;
	return(_contains((void*)L));
} // contains

void **IntHash::keys(void) {
	int i;
	int *key_list = (int*)_keys();
	
	for (i = 0; ; i++) {
		key_list[i] -= 1;
		if (key_list[i] == -1) break;
	} // for
	
	return((void**)key_list);
} // keys()

bool IntHash::remove(const int key) {
	int L = key + 1;
	return(_remove((void*)L));
} // remove()

