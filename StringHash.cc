/* $Id: StringHash.cc,v 1.1 2003/10/07 16:42:39 blytkerchan Exp $ */
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
#else  // in native windows
#define HAVE_STRING_H 1
#endif
#include <assert.h>
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
#include "Hash.h"
#include "StringHash.h"

/***
// Constants */
static const char *HASH_MAGIC = "HASS";
static const char *HASH_VERSION = "0100";

/*** 
// Constructors */
StringHash::StringHash() : Hash() {} // StringHash()
StringHash::StringHash(ulong _u) : Hash(_u) {} // StringHash()

/*** 
// Destructors */
StringHash::~StringHash() {} // ~StringHash()

/*** 
// Class functions */
bool StringHash::empty_key(void *key) {
	char *K = (char*)key;
	
	if (K == NULL) return (true);
	return(K[0] == 0);
} // empty_key()

/** 
// cmp_keys(): compare two keys, return 0 if they're the same, nonzero if not
*/
int StringHash::cmp_keys(const void *key1, const void *key2) {
	char *K1 = (char*)key1, *K2 = (char*)key2;
	
	if (K1 == K2) return (0);
	if ((K1 == NULL) || (K2 == NULL)) return (-1);

	return (strcmp(K1, K2));
} // cmp_keys()

ulong StringHash::hash(const void *key) {
	char *K = (char*)key;
	ulong currentAdler1 = 0x00000000UL;
    	ulong currentAdler2 = 0x00000001UL;
    	ulong i;
    	
    	for (i = 0; i < strlen(K); i++) {
    		currentAdler1 += K[i];
    		currentAdler2 += currentAdler1;
    	} // for
    	currentAdler1 = currentAdler1 << 16;
    	
    	return (currentAdler1 + currentAdler2);
} // hash()

bool StringHash::_write(void) {
	char **keys;
	int i, data_size;
	FILE *file;
	void *data = NULL;
	
	assert(sizeof(int) == 4);
	if ((file = fopen(filename, "wb")) == NULL) return(false);
	fwrite(HASH_MAGIC, 4, 1, file);
	fwrite(HASH_VERSION, 4, 1, file);
	keys = (char**)this->keys();
	for (i = 0; keys[i]; i++) {
		data_size = strlen(keys[i]) + 1;
		fwrite(&data_size, 4, 1, file);
		fwrite(keys[i], data_size, 1, file);
		data = this->get(keys[i]);
		data_size = write_helper_function(data);
		fwrite(&data_size, 4, 1, file);
		fwrite(data, data_size, 1, file);
	} // for
	fclose(file);
	
	return(true);
} // _write()

bool StringHash::read(char *filename) {
	strcpy(this->filename, filename);
	FILE *file;
	int data_size;
	key_type key;
	char magic[4];
	void *data = NULL;
	
	assert(sizeof(int) == 4);
	if ((file = fopen(filename, "rb")) == NULL) return(false);
	// check the magic number
	fread(magic, 4, 1, file);
	if (memcmp(magic, HASH_MAGIC, 4) != 0) return(false);
	fread(magic, 4, 1, file);
	if (memcmp(magic, HASH_VERSION, 4) != 0) return(false);
	key.value = NULL; // just to make sure
	while(fread(&data_size, 4, 1, file) == 1) {
		key.size = data_size;
		key.value = malloc(key.size);
		fread(key.value, key.size, 1, file);
		fread(&data_size, 4, 1, file);
		data = malloc(data_size);
		fread(data, data_size, 1, file);
		this->put(key.value, data);
	} // while
	fclose(file);
	
	return(true);
} // read()
