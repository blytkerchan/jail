/* $Id: IniHash.cc,v 1.1 2003/10/07 16:42:39 blytkerchan Exp $ */
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
#include <assert.h>
#if !defined(_WIN32) || defined (__CYGWIN__)
#include <config.h>
#else  // in native windows
#define HAVE_STRING_H 1
#define strlwr _strlwr
#define HAVE_STRLWR 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Hash.h"
#include "StringHash.h"
#include "NocaseStringHash.h"
#include "IniHash.h"
#if !HAVE_STRLWR
	#include <libswing/strlwr.h>
#endif // !HAVE_STRLWR

/*** 
// Constructors */
IniHash::IniHash() : NocaseStringHash() {} // IniHash()
IniHash::IniHash(ulong _u) : NocaseStringHash(_u) {} // IniHash()

/*** 
// Destructors */
IniHash::~IniHash() {} // ~IniHash()

bool IniHash::read(char * _filename) {
	FILE *ini_file;
	char buffer[INI_LINE_MAX]; 
	char *ptr, *new_hash_name, *curr_hash_name = NULL, *key_name = NULL, *value;
	static NocaseStringHash *ncs = NULL;
	
	// open the inifile
	if ((ini_file = fopen(_filename, "rt")) == NULL) {
		return(false);
	} // if
	// read it line by line
	while (fgets(buffer, INI_LINE_MAX, ini_file) != NULL) {
		ptr = strchr(buffer, '\n');
		if (ptr) *ptr = 0;
		ptr = strchr(buffer, '#');
		if (ptr) *ptr = 0;
		ptr = buffer;
		while ((*ptr == ' ') || (*ptr == '\t')) ptr++; // skip leading spaces
		if (*ptr == 0) continue;
		if (*ptr == '[') {
			new_hash_name = ptr + 1;
			while ((*ptr != ']') && (*ptr != 0)) ptr++;
			if (*ptr == 0) continue;
			*ptr = 0;
			curr_hash_name = strdup(new_hash_name);
			continue;
		} // if
		if (!curr_hash_name) continue;
		ncs = (NocaseStringHash*)_get(curr_hash_name);
		if (!ncs) ncs = new NocaseStringHash();
		value = strchr(ptr, '=');
		if (!value) continue;
		key_name = ptr;
		if (*ptr == '(') {
			ptr++;
			while ((*ptr == ' ') || (*ptr == '\t')) ptr++; // skip leading spaces (after '(')
			key_name = ptr;
			ptr = strchr(key_name, ')');
			*ptr = 0;
			ptr = key_name + strlen(key_name) - 1;
			while ((*ptr == ' ') || (*ptr == '\t')) ptr--; // skip trailing spaces
			ptr++;
			*ptr = 0;
		} // if
		ptr = value;
		ptr--; // skip the equal sign;
		while (*ptr && ((*ptr == ' ') || (*ptr == '\t'))) ptr--; // skip trailing spaces
		ptr++;
		*ptr = 0;
		value++; // skip the equal sign
		while ((*value == ' ') || (*value == '\t')) value++;
		ptr = value + strlen(value) - 1;
		while ((*ptr == ' ') || (*ptr == '\t')) ptr--; // skip trailing spaces
		ptr++;
		*ptr = 0;
		key_name = strdup(key_name);
		value = strdup(value);
		this->put(curr_hash_name, key_name, value);
	} // while
	// and close it
	fclose(ini_file);
	// as all went well, store the filename
	strcpy(filename, _filename);

	return(true);
} // read()

bool IniHash::_write(void) {
	key_list_type **keys;
	ulong p, q;
	FILE *ini_file = NULL;
	char *buffer;

	if ((ini_file = fopen(filename, "wt")) == NULL) return(false);
	keys = (key_list_type **)this->keys();
	for (p = 0; keys[p]; p++) {
		fprintf(ini_file, "[%s]\n", keys[p]->primary_key);
		for (q = 0; keys[p]->secondary_keys[q]; q++) {
			buffer = (char*)this->get(keys[p]->primary_key, keys[p]->secondary_keys[q]);
			fprintf(ini_file, "%s = %s\n", keys[p]->secondary_keys[q], buffer);
		} // for
	} // for
	assert(p == Count);
	fclose(ini_file);

	return(true);
} // write()

void * IniHash::get (const char * primary_key, const char * secondary_key) {
	NocaseStringHash *ncs = NULL;
	
	ncs = (NocaseStringHash *)this->_get(primary_key);
	if (!(ncs)) return(NULL);
	return(ncs->get(secondary_key));
} // get()

bool IniHash::put (const char * primary_key, const char * secondary_key, char *value) {
	static NocaseStringHash *ncs = NULL;
	
	ncs = (NocaseStringHash *)this->NocaseStringHash::get(primary_key);
	if (!(ncs)) {
		ncs = new NocaseStringHash();
		if (!(ncs)) return(false);
		this->_put(primary_key, ncs);
	} // unless
	return(ncs->put(secondary_key, value));
} // put()

void **IniHash::keys(void) {
	static key_list_type **keys = NULL;
	char **primary_keys;
	NocaseStringHash *ncs = NULL;
	unsigned long int i;

	keys = (key_list_type **)malloc((Count  + 1) * sizeof(key_list_type*));
	memset(keys, 0, (Count  + 1) * sizeof(key_list_type*));
	primary_keys = (char**)this->_keys();
	for (i = 0; i < Count; i++) {
		keys[i] = (key_list_type*)malloc(sizeof(key_list_type));
		keys[i]->primary_key = primary_keys[i];
		ncs = (NocaseStringHash*)this->NocaseStringHash::get(primary_keys[i]);
		keys[i]->secondary_keys = (char**)ncs->keys();
	} // for
	
	return ((void**)keys);
} // keys()

bool IniHash::contains(const char * primary_key, const char * secondary_key) {
	NocaseStringHash *ncs = NULL;

	ncs = (NocaseStringHash*)this->NocaseStringHash::get(primary_key);
	if (!ncs) return (false);
	
	return(ncs->contains(secondary_key));
} // contains()

bool IniHash::remove(const char *primary_key, const char *secondary_key) {
	NocaseStringHash *ncs = NULL;

	ncs = (NocaseStringHash*)this->NocaseStringHash::get(primary_key);
	if (!ncs) return (false);
	
	return(ncs->remove(secondary_key));
} // remove()
