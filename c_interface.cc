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
#include "hash.h"
#include "Hash.h"
#include "StringHash.h"
#include "IntHash.h"

extern "C" Hash * cxx_new_hash(libhash_hashtype hash_type)
{
	switch (hash_type)
	{
	case NORMAL_HASH :
		return(new Hash);
	case STRING_HASH :
		return(new StringHash);
	case INT_HASH :
		return(new IntHash);
	default :
		return(NULL);
	}
}

extern "C" void cxx_delete_hash(Hash * hash)
{
	delete hash;
}

extern "C" void *cxx_hash_get(Hash * hash, const void *key)
{
	return(hash->get
	       (key));
}

extern "C" int cxx_hash_put(Hash * hash, const void *key, const void *value)
{
	return(hash->put(key, value) ? 0 : 1);
}

extern "C" int cxx_hash_remove(Hash * hash, const void *key)
{
	return(hash->remove
	       (key) ? 0 : 1);
}

extern "C" void ** cxx_hash_keys(Hash * hash)
{
	return(hash->keys());
}

extern "C" void cxx_hash_for_each(Hash * hash, libcontain_foreach2_func_t func, void * data)
{
	hash->for_each((void (*)(void*, void*, void*))func, data);
}

