/* $Id: IntHash.h,v 1.1 2003/10/07 16:42:39 blytkerchan Exp $ */
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
#ifndef SLIB_LIBHASH_INTHASH_H
#define SLIB_LIBHASH_INTHASH_H

/*** 
// Includes */
#include "Hash.h"

#ifdef __cplusplus
/*** 
// Class declaration */
class IntHash : public Hash {
public:
	IntHash();
	IntHash(ulong);
	bool read(char*);
	void *get(const int key);
	bool put(const int key, const void *value);
	bool contains(const int key);
	void **keys(void);
	bool remove(int);
	
protected:
	bool _write(void);
	bool empty_key(void *key);
	int cmp_keys(const void *, const void *);
	ulong hash(const void *);
}; // StringHash


#endif // __cplusplus

#endif // SLIB_LIBHASH_INTHASH_H
