/* $Id: NocaseStringHash.cc,v 1.1 2003/10/07 16:42:39 blytkerchan Exp $ */
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
#define strlwr _strlwr
#define HAVE_STRLWR 1
#endif
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
#include "NocaseStringHash.h"
#if !HAVE_STRLWR
	#include <libswing/strlwr.h>
#endif // !HAVE_STRLWR

/*** 
// Constructors */
NocaseStringHash::NocaseStringHash() : StringHash() {} // NocaseStringHash()
NocaseStringHash::NocaseStringHash(ulong _u) : StringHash(_u) {} // NocaseStringHash()
NocaseStringHash::NocaseStringHash(StringHash *sh) : StringHash(*sh) {} // NocaseStringHash()
/*** 
// Destructors */
NocaseStringHash::~NocaseStringHash() {} // ~StringHash()

/*** 
// Class functions */
/** 
// cmp_keys(): compare two keys, return 0 if they're the same, nonzero if not
*/
int NocaseStringHash::cmp_keys(const void *key1, const void *key2) {
	char *K1 = (char*)key1, *K2 = (char*)key2;
	
	if (K1 == K2) return (0);
	if ((K1 == NULL) || (K2 == NULL)) return (-1);
	K1 = strlwr(K1);
	K2 = strlwr(K2);
	
	return (strcmp(K1, K2));
} // cmp_keys()

ulong NocaseStringHash::hash(const void *key) {
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
