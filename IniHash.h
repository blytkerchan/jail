/* $Id: IniHash.h,v 1.4 2004/04/08 15:02:30 blytkerchan Exp $ */
/* Jail: Just Another Interpreted Language
 * Copyright (c) 2003-2004, Ronald Landheer-Cieslak
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
// The IniHash is designed to read .ini files into a two-layered 
// hash, and write .ini files from such hashes
#ifndef SLIB_LIBHASH_INISTRINGHASH_H
#define SLIB_LIBHASH_INISTRINGHASH_H

#ifdef __cplusplus


/*** 
// Includes */
#include "NocaseStringHash.h"

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
