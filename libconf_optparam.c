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
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "libconf.h"
#include "libreplace/catstr.h"
#include "libreplace/macros.h"

libconf_optparam_t * libconf_optparam_dup(const libconf_optparam_t * param)
{
	libconf_optparam_t * retval;

	retval = (libconf_optparam_t *)malloc(sizeof(libconf_optparam_t));
	retval->name = strdup(param->name);
	retval->param_type = param->param_type;
	switch (param->param_type)
	{
	case PT_FILENAME :
	case PT_STRING :
		retval->val.str_val = strdup(param->val.str_val);
		break;
	case PT_YESNO :
	case PT_TRUEFALSE :
		retval->val.bool_val = param->val.bool_val;
		break;
	case PT_NUMERIC_LIST :
	case PT_STRING_LIST :
	case PT_FILENAME_LIST :
		retval->val.array_val = array_copy(param->val.array_val);
		break;
	default :
		retval->val.num_val = param->val.num_val;
	}
	retval->have_error = param->have_error;

	return retval;
}

libconf_optparam_t ** libconf_optparams_dup(const libconf_optparam_t ** params)
{
	int n, i;
	libconf_optparam_t ** retval;

	for (n = 0; params[n]; n++);
	retval = calloc(n + 1, sizeof(libconf_optparam_t*));
	for (i = 0; i < n; i++)
	{
		retval[i] = libconf_optparam_dup(params[i]);
	}

	return retval;
}

void libconf_optparam_free(libconf_optparam_t * param)
{
	size_t n, i;
	void * p;
	
	free(param->name);
	switch (param->param_type)
	{
	case PT_STRING :
	case PT_FILENAME :
		free(param->val.str_val);
		break;
	case PT_STRING_LIST :
	case PT_FILENAME_LIST :
		n = array_get_size(param->val.array_val);
		for (i = 0; i < n; i++)
		{
			if ((p = array_get(param->val.array_val, i)) != NULL)
			{
				free(p);
			}
		}
	case PT_NUMERIC_LIST :
		free_array(param->val.array_val);
		break;
	default :
		break;
	}

	free(param);
}

void libconf_optparams_free(libconf_optparam_t ** params)
{
	libconf_optparam_t * p;
	int i = 0;

	while ((p = params[i++]) != NULL)
		libconf_optparam_free(p);
	
	free(params);
}

libconf_optparam_t * libconf_optparam_new(
	char * name,
	libconf_param_type_t param_type,
	char * str)
{
	libconf_optparam_t * retval = (libconf_optparam_t*)malloc(sizeof(libconf_optparam_t));
	memset(retval, 0, sizeof(libconf_optparam_t));
	
	retval->name = strdup(name);
	retval->param_type = param_type;
	switch (retval->param_type)
	{
	case PT_NONE : /* we do expect a param, but we don't want it.. */
		break;
	case PT_YESNO :
		if (strcmp(str, "no") == 0)
			retval->val.bool_val = 0;
		else
		{
			retval->val.bool_val = 1;
			if (strcmp(str, "yes") != 0)
				retval->have_error = 1;
		}
		break;
	case PT_TRUEFALSE :
		if (strcmp(str, "false") == 0)
			retval->val.bool_val = 0;
		else
		{
			retval->val.bool_val = 1;
			if (strcmp(str, "true") != 0)
				retval->have_error = 1;
		}
		break;
	case PT_NUMERIC :
		retval->val.num_val = atoi(str);
		break;
	case PT_STRING :
	case PT_FILENAME :
		retval->val.str_val = trim(strdup(str));
		break;
	case PT_NUMERIC_LIST :
		retval->val.array_val = new_array(0);
		array_push_back(retval->val.array_val, (void*)atoi(str));
		break;
	case PT_STRING_LIST :
	case PT_FILENAME_LIST :
		retval->val.array_val = new_array(0);
		array_push_back(retval->val.array_val, trim(strdup(str)));
		break;
	default :
		free(retval);
		return NULL;
	}

	return retval;
}

