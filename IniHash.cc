/* $Id: IniHash.cc,v 1.5 2004/05/07 16:28:21 blytkerchan Exp $ */
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
/***
// Includes */
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Hash.h"
#include "StringHash.h"
#include "NocaseStringHash.h"
#include "IniHash.h"

/***
// Constructors */
IniHash::IniHash() : NocaseStringHash()
{} // IniHash()
IniHash::IniHash(unsigned int _u) : NocaseStringHash(_u)
{} // IniHash()

/***
// Destructors */
IniHash::~IniHash()
{} // ~IniHash()

bool IniHash::read(char * _filename)
{
	FILE *ini_file;
	char buffer[INI_LINE_MAX];
	char *ptr, *new_hash_name, *curr_hash_name = NULL, *key_name = NULL, *value;
	static NocaseStringHash *ncs = NULL;

	reg_reader();

	// open the inifile
	if ((ini_file = fopen(_filename, "rt")) == NULL)
	{
		ureg_reader();
		return(false);
	} // if
	// read it line by line
	while (fgets(buffer, INI_LINE_MAX, ini_file) != NULL)
	{
		ptr = strchr(buffer, '\n');
		if (ptr)
			*ptr = 0;
		ptr = strchr(buffer, '#');
		if (ptr)
			*ptr = 0;
		ptr = buffer;
		while ((*ptr == ' ') || (*ptr == '\t'))
			ptr++; // skip leading spaces
		if (*ptr == 0)
			continue;
		if (*ptr == '[')
		{
			new_hash_name = ptr + 1;
			while ((*ptr != ']') && (*ptr != 0))
				ptr++;
			if (*ptr == 0)
				continue;
			*ptr = 0;
			curr_hash_name = strdup(new_hash_name);
			continue;
		} // if
		if (!curr_hash_name)
			continue;
		ncs = (NocaseStringHash*)_get(curr_hash_name);
		if (!ncs)
			ncs = new NocaseStringHash();
		value = strchr(ptr, '=');
		if (!value)
			continue;
		key_name = ptr;
		if (*ptr == '(')
		{
			ptr++;
			while ((*ptr == ' ') || (*ptr == '\t'))
				ptr++; // skip leading spaces (after '(')
			key_name = ptr;
			ptr = strchr(key_name, ')');
			*ptr = 0;
			ptr = key_name + strlen(key_name) - 1;
			while ((*ptr == ' ') || (*ptr == '\t'))
				ptr--; // skip trailing spaces
			ptr++;
			*ptr = 0;
		} // if
		ptr = value;
		ptr--; // skip the equal sign;
		while (*ptr && ((*ptr == ' ') || (*ptr == '\t')))
			ptr--; // skip trailing spaces
		ptr++;
		*ptr = 0;
		value++; // skip the equal sign
		while ((*value == ' ') || (*value == '\t'))
			value++;
		ptr = value + strlen(value) - 1;
		while ((*ptr == ' ') || (*ptr == '\t'))
			ptr--; // skip trailing spaces
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

	ureg_reader();
	return(true);
} // read()

bool IniHash::_write(void)
{
	key_list_type **keys;
	unsigned int p, q;
	FILE *ini_file = NULL;
	char *buffer;

	if ((ini_file = fopen(filename, "wt")) == NULL)
		return(false);
	keys = (key_list_type **)this->keys();
	for (p = 0; keys[p]; p++)
	{
		fprintf(ini_file, "[%s]\n", keys[p]->primary_key);
		for (q = 0; keys[p]->secondary_keys[q]; q++)
		{
			buffer = (char*)this->get
			         (keys[p]->primary_key, keys[p]->secondary_keys[q]);
			fprintf(ini_file, "%s = %s\n", keys[p]->secondary_keys[q], buffer);
		} // for
	} // for
	assert(p == Count);
	fclose(ini_file);

	return(true);
} // write()

void * IniHash::get(const char * primary_key, const char * secondary_key)
{
	NocaseStringHash *ncs = NULL;
	void * retval;

	reg_reader();
	ncs = (NocaseStringHash *)this->_get(primary_key);
	if (!(ncs))
	{
		ureg_reader();
		return(NULL);
	}
	retval = ncs->get(secondary_key);
	ureg_reader();
	
	return retval;
} // get()

bool IniHash::put (const char * primary_key, const char * secondary_key, char *value)
{
	static NocaseStringHash *ncs = NULL;
	bool retval = false;

	reg_reader();
	ncs = (NocaseStringHash *)this->NocaseStringHash::get(primary_key);
	if (!(ncs))
	{
		ncs = new NocaseStringHash();
		if (!(ncs))
		{
			ureg_reader();
			return(false);
		}
		retval = this->_put(primary_key, ncs);
	} // unless
	retval |= ncs->put(secondary_key, value);
	ureg_reader();

	return retval;
} // put()

void **IniHash::keys(void)
{
	static key_list_type **keys = NULL;
	char **primary_keys;
	NocaseStringHash *ncs = NULL;
	unsigned long int i;

	reg_reader();
	keys = (key_list_type **)malloc((Count  + 1) * sizeof(key_list_type*));
	memset(keys, 0, (Count  + 1) * sizeof(key_list_type*));
	primary_keys = (char**)this->_keys();
	for (i = 0; i < Count; i++)
	{
		keys[i] = (key_list_type*)malloc(sizeof(key_list_type));
		keys[i]->primary_key = primary_keys[i];
		ncs = (NocaseStringHash*)this->NocaseStringHash::get
			      (primary_keys[i]);
		keys[i]->secondary_keys = (char**)ncs->keys();
	} // for
	ureg_reader();
	
	return ((void**)keys);
} // keys()

bool IniHash::contains(const char * primary_key, const char * secondary_key)
{
	NocaseStringHash *ncs = NULL;
	bool retval;

	ncs = (NocaseStringHash*)this->NocaseStringHash::get(primary_key);
	if (!ncs)
		retval = false;
	else
		retval = ncs->contains(secondary_key);

	ureg_reader();

	return retval;
} // contains()

bool IniHash::remove
	(const char *primary_key, const char *secondary_key)
{
	NocaseStringHash *ncs = NULL;
	bool retval;
	
	reg_reader();	
	ncs = (NocaseStringHash*)this->NocaseStringHash::get(primary_key);
	if (!ncs)
		retval = false;
	else
		retval = ncs->remove(secondary_key);
	ureg_reader();

	return retval;
} // remove()

