/* $Id: IniHash.h,v 1.1 2003/10/07 16:42:39 blytkerchan Exp $ */
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
/* BLAHBLAHBLAH 
// The IniHash is designed to read .ini files into a two-layered 
// hash, and write .ini files from such hashes
*/
#ifndef SLIB_LIBHASH_INISTRINGHASH_H
#define SLIB_LIBHASH_INISTRINGHASH_H

#ifdef __cplusplus


/*** 
// Includes */
#include <libhash/NocaseStringHash.h>

/*** 
// Defines */
#define INI_LINE_MAX 256 // mamimal length of a line in a .ini file

/*** 
// Type definitions */
typedef struct _key_list_type {
	char *primary_key;
	char **secondary_keys;
} key_list_type;

/*** 
// Class declaration */
class IniHash : public NocaseStringHash {
public:
	IniHash();
	IniHash(ulong);
	~IniHash();
	virtual bool read(char * filename);
	virtual bool _write(void);
	virtual void * get (const char * primary_key, const char * secondary_key);
	virtual bool put (const char * primary_key, const char * secondary_key, char *value);
	virtual void **keys(void);
	virtual bool contains(const char * primary_key, const char * secondary_key);
	virtual bool remove(const char * primary_key, const char * secondary_key);
}; // IniHash


#endif // __cplusplus

#endif // SLIB_LIBHASH_INISTRINGHASH_H