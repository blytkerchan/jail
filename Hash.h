/* $Id: Hash.h,v 1.3 2004/04/01 13:35:10 blytkerchan Exp $ */
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
	typedef void (*foreach_func_t)(void*key,void*val,void*data);
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
	virtual void for_each(foreach_func_t func, void * user_data);
	
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
