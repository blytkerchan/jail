/* $Id: StringHash.h,v 1.1 2003/10/07 16:42:39 blytkerchan Exp $ */
/* Managing Hash tables with stringd as keys
   Copyright (C) 2002
   Ronald Landheer <info@rlsystems.net>

This file is part of the Real Life Systems Socket Library (slib).

slib is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

slib is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with slib; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */
#ifndef SLIB_LIBHASH_STRINGHASH_H
#define SLIB_LIBHASH_STRINGHASH_H

#ifdef __cplusplus


/*** 
// Includes */
#include "Hash.h"

/*** 
// Class declaration */
class StringHash : public Hash {
public:
	StringHash();
	StringHash(ulong);
	~StringHash();
	bool read(char*);
	
protected:
	bool _write(void);
	bool empty_key(void *key);
	int cmp_keys(const void *, const void *);
	ulong hash(const void *);
}; // StringHash


#endif // __cplusplus

#endif // SLIB_LIBHASH_STRINGHASH_H
