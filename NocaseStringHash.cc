/* $Id: NocaseStringHash.cc,v 1.3 2004/02/03 13:53:32 blytkerchan Exp $ */
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
#include "libreplace/strlwr.h"
#endif

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
