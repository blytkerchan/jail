/* $Id: Hash.h,v 1.1 2003/10/07 16:42:39 blytkerchan Exp $ */
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
#ifndef SLIB_LIBHASH_HASH_H
#define SLIB_LIBHASH_HASH_H

/*** 
// Includes */
#include <stdio.h>

/*** 
// Defines */
#define ulong unsigned long /// NOTE: Get the size of this during configuration - portability issue

/*** 
// Type definitions */
typedef struct _key_type {
	int size;
	void *value;
} key_type;

typedef struct _mapping_type {
	void *key;
	void *value;
} mapping_type;

#ifndef __cplusplus
typedef struct Hash Hash;
#else
/*** 
// Class definition */
class Hash {
public:
	Hash();
	Hash(ulong);
	virtual ~Hash();
	virtual void *get(const void *key);
	virtual bool put(const void *key, const void *value);
	virtual bool remove(const void *key);
	bool clear(void);
	ulong count(void);
	virtual void **keys(void);
	virtual bool contains (const void *key);
	virtual bool read(char* filename);
	virtual bool write(char *filename, int (*helper_function)(void*));
	virtual bool write(int (*helper_function)(void*));
	virtual bool write(char *filename);
	virtual bool write(void);
	void set_write_helper_function(int (*helper_function)(void*));
	void * get_write_helper_function(void); // cast to int (*)(void*)
	char *get_filename(void);
	void set_filename(char *filename);
	
protected:
	virtual int cmp_keys(const void *key1, const void *key2);
	virtual ulong hash(const void *key);
	bool grow(int size);
	virtual bool empty_key(void *key);
	virtual void *_get(const void *key);
	virtual bool _put(const void *key, const void *value);
	virtual bool _contains (const void *key);
	virtual void **_keys(void);
	virtual bool _write(void);
	virtual bool _remove(const void *key);
	
	int (*write_helper_function)(void*);
	ulong size;  // size of the hash
	ulong Count; // number of elements in the hash
	char filename[FILENAME_MAX + 1];
	mapping_type *mappings;
}; // Hash
#endif // __cplusplus

#endif // SLIB_LIBHASH_HASH_H
