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
/* Some important implementation notes:
 * The library is the owner if everything that is in each handle. That means 
 * that every field in the handle is freed when the handle itself is freed. It
 * also means that *this* library is the owner if everything that is in the 
 * ADTs, because libcontain does not own what is in its containers (it only
 * provides the containers). That means that whenever we copy an ADT, we have 
 * to copy its contents as well - i.e. do a "deep" copy.
 * Due to this policy, overwriting alread-entered entries in a hash will result
 * in memory leaks as we currently don't check whether a value already exists in
 * the hash before putting a new value in.
 */
#include <sys/types.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "libconf.h"
#include "libconf_config.h"
#include "libreplace/catstr.h"
#include "libcontain/hash.h"
#include "libcontain/hash_helpers.h"

libconf_t * libconf_init(
	const char * global_config_filename,
	const char * local_config_filename,
	const libconf_opt_t ** options,
	const libconf_optparam_t ** defaults,
	int argc, const char ** argv)
{
	int i;
	libconf_opt_t ** t_options;
	libconf_t * retval = (libconf_t *)malloc(sizeof(libconf_t));
	memset(retval, 0, sizeof(libconf_t));

	if (global_config_filename != NULL)
		retval->global_config_filename = strdup(global_config_filename);
	if (local_config_filename != NULL)
		retval->local_config_filename = strdup(local_config_filename);
	retval->options = hash_new(0, hash_hlp_string_hash, hash_hlp_string_cmp);
	t_options = libconf_defaultopts();
	for (i = 0; t_options[i]; i++)
	{
		hash_put(retval->options, strdup(t_options[i]->co_name), t_options[i]);
	}
	free(t_options); // note: shallow free because the contents is in the hash
	t_options = libconf_optdup(options);
	for (i = 0; t_options[i]; i++)
	{
		hash_put(retval->options, strdup(t_options[i]->co_name), t_options[i]);
	}
	free(t_options); // note: shallow free because the contents is in the hash
	retval->option_hash = hash_new(0, hash_hlp_string_hash, hash_hlp_string_cmp);
	retval->tmp_hash = hash_new(0, hash_hlp_string_hash, hash_hlp_string_cmp);
	retval->argc = argc;
	retval->argv = (char**)calloc(argc + 1, sizeof(char*));
	for (i = 0; i < argc; i++)
		retval->argv[i] = strdup(argv[i]);
	if (defaults)
		retval->defaults = libconf_optparams_dup(defaults);

	return retval;
}

void libconf_fini_helper(const void * key, const void * val, void * data)
{
	free((void*)key);
	free((void*)val);
}

void libconf_fini(libconf_t * handle)
{
	int i;

	free(handle->global_config_filename);
	free(handle->local_config_filename);

	hash_foreach(handle->options, libconf_fini_helper, NULL);
	hash_free(handle->options);
	hash_foreach(handle->option_hash, libconf_fini_helper, NULL);
	hash_free(handle->option_hash);
	hash_foreach(handle->tmp_hash, libconf_fini_helper, NULL);
	hash_free(handle->tmp_hash);
	for (i = 0; i < handle->argc; i++)
		free(handle->argv[i]);
	free(handle->argv);
	if (handle->defaults)
		libconf_optparams_free(handle->defaults);
	free(handle);
}

typedef enum { 
	ET_NONE, ET_EXPECTED_PARAM_TP_NOT_FOUND, ET_PARAM_MALFORMED, ET_EXPECTED_OPTION_TP_NOT_FOUND, ET_UNKNOWN_OPTION
} libconf_phase_error_t;

int libconf_phase1_helper1(const void * v1, const void * v2)
{
	libconf_opt_t * val1 = (libconf_opt_t *)v1;
	libconf_opt_t * val2 = (libconf_opt_t *)v2;
	
	return val1->co_short_opt - val2->co_short_opt;
}

/* read the command-line options and set the global_config_filename and 
 * local_config_filename fields accordingly */
int libconf_phase1(libconf_t * handle)
{
	int argc = handle->argc;
	char ** argv = handle->argv;
	libconf_phase_error_t have_error;
	libconf_optparam_t * param;
	libconf_do_on_error_t error_action = DOE_ERROR;
	int have_param;
	char * param_name;
	char * tmp_str;
	libconf_opt_t * option;
	libconf_opt_t t_option;

	handle->argv0 = *argv;
	argv++; argc--;
	while (argc > 0)
	{
		have_error = PT_NONE;
		param = NULL;
		have_param = 0;
		tmp_str = NULL;

		switch (argv[0][0])
		{
		case '-' :
			switch (argv[0][1])
			{
			case '-' : /* we have a long option */
				option = hash_get(handle->options, argv[0] + 2);
				if (option != NULL)
				{ /* we've found the option */
					param_name = option->co_name;
					error_action = option->do_on_error;
					if (option->co_long_takes_param != TP_NO)
					{
						if (argc > 1 && argv[1][0] && argv[1][0] != '-')
						{ /* we have our option's param */
							switch (option->co_long_param_type)
							{
							case PT_NUMERIC_LIST :
							case PT_STRING_LIST :
							case PT_FILENAME_LIST :
								param = hash_get(handle->tmp_hash, option->co_name);
								if (param != NULL)
									vector_push_back(param->val.vector_val, strdup(argv[1]));
							default :
								if (param == NULL)
									param = libconf_optparam_new(option->co_name, option->co_long_param_type, argv[1]);
							}
							if (param == NULL)
								have_error = ET_PARAM_MALFORMED;
							else if (param->have_error)
								have_error = ET_PARAM_MALFORMED;
							have_param = 1;
						}
						else if (option->co_long_takes_param == TP_YES)
						{ /* we should have had a param, but we don't */
							have_error = ET_EXPECTED_PARAM_TP_NOT_FOUND;
						}
					}
					hash_put(handle->tmp_hash, strdup(option->co_name), param);
				}
				else
				{
					have_error = ET_UNKNOWN_OPTION;
				}
				break;
			case 0 : /* we should stop treating command-line options and 
			          * concatenate the rest of the command-line, separated by 
						 * spaces. */
				argv++; argc--;
				while (argc)
				{
					argv++; argc--;
					tmp_str = catstr(tmp_str, argv[0]);
				}
				param = libconf_optparam_new("-", PT_STRING, tmp_str);
				free(tmp_str);
				hash_put(handle->tmp_hash, strdup("-"), param);
				break;
			default : /* we have a short option */
				t_option.co_short_opt = argv[0][1];
				option = hash_search(handle->options, &t_option, libconf_phase1_helper1);
				if (option != NULL)
				{	/* we have our option */
					param_name = option->co_name;
					error_action = option->do_on_error;
					if (option->co_short_takes_param != TP_NO)
					{
						if (argv[0][2])
						{ /* we have a parameter directly following */
							switch (option->co_short_param_type)
							{
							case PT_NUMERIC_LIST :
							case PT_STRING_LIST :
							case PT_FILENAME_LIST :
								param = hash_get(handle->tmp_hash, option->co_name);
								if (param != NULL)
									vector_push_back(param->val.vector_val, strdup(argv[0] + 2));
							default :
								if (param == NULL)
									param = libconf_optparam_new(option->co_name, option->co_short_param_type, argv[0] + 2);
							}
							if (param == NULL)
								have_error = ET_PARAM_MALFORMED;
							else if (param->have_error)
								have_error = ET_PARAM_MALFORMED;
							// have_param = 1; we don't count these
						}
						else if (argc > 1 && argv[1][0] && argv[1][0] != '-' )
						{	/* we have a parameter */
							switch (option->co_short_param_type)
							{
							case PT_NUMERIC_LIST :
							case PT_STRING_LIST :
							case PT_FILENAME_LIST :
								param = hash_get(handle->tmp_hash, option->co_name);
								if (param != NULL)
									vector_push_back(param->val.vector_val, strdup(argv[1]));
							default :
								if (param == NULL)
									param = libconf_optparam_new(option->co_name, option->co_short_param_type, argv[1]);
							}
							if (param == NULL)
								have_error = ET_PARAM_MALFORMED;
							else if (param->have_error)
								have_error = ET_PARAM_MALFORMED;
							have_param = 1;
						}
						else if (option->co_short_takes_param == TP_YES)
						{	/* we should have had a param, but we don't */
							have_error = ET_EXPECTED_PARAM_TP_NOT_FOUND;
						}
					}
					hash_put(handle->tmp_hash, strdup(option->co_name), param);
				}
				else
				{
					have_error = ET_UNKNOWN_OPTION;
				}
				break;
			}
			break;
		default : /* no option? */
			have_error = ET_EXPECTED_OPTION_TP_NOT_FOUND;
			break;
		}

		if ((have_error != ET_NONE) && (error_action != DOE_NOTHING))
		{
			switch (error_action)
			{
			case DOE_WARNING : 
				fprintf(stderr, "%s: Warning: ", handle->argv0);
				break;
			case DOE_ERROR :
				fprintf(stderr, "%s: Error: ", handle->argv0);
				break;
			default :
				break;
			}
			switch (have_error)
			{
			case ET_EXPECTED_PARAM_TP_NOT_FOUND :
				fprintf(stderr, "expected parameter not found for option %s\n", argv[0]);
				break;
			case ET_PARAM_MALFORMED :
				fprintf(stderr, "parameter malformed for option %s\n", argv[0]);
				break;
			case ET_EXPECTED_OPTION_TP_NOT_FOUND :
				fprintf(stderr, "not an option %s\n", argv[0]);
				break;
			case ET_UNKNOWN_OPTION :
				fprintf(stderr, "unknown option %s\n", argv[0]);
				break;
			default :
				break;
			}
			if (error_action == DOE_ERROR)
				exit(1);
		}
		
		if (have_param)
		{
			argv++; argc--;
		}
		argv++; argc--;
	}

	return 0;
}

/* set the global defaults */
int libconf_phase2(libconf_t * handle)
{
	int i;

	if (handle->defaults == NULL)
		return 0;
	for (i = 0; handle->defaults[i]; i++)
		hash_put(handle->option_hash, strdup(handle->defaults[i]->name), handle->defaults[i]);
	
	return 0; 
}

/* parse the global config file */
int libconf_phase3(libconf_t * handle) 
{ 
	return libconf_readconf_global(handle);
}

int libconf_phase4(libconf_t * handle) 
{ 
	return libconf_readconf_local(handle);
}

static void libconf_phase5_helper(const void * key, const void * val, void * data)
{
	libconf_t * handle = (libconf_t *)data;
	hash_put(handle->option_hash, strdup((char*)key), (void*)val);
}
int libconf_phase5(libconf_t * handle) 
{
	hash_foreach(handle->tmp_hash, libconf_phase5_helper, handle);

	return 0;
}

int libconf_getopt(libconf_t * handle, const char * optname, ...)
{
	va_list ap;
	libconf_optparam_t * val;
	int rc = 0;
	int * d;
	char ** s;
	vector_t ** a;

	val = (libconf_optparam_t*)hash_get(handle->option_hash, (void*)optname);
	if (val == NULL)
		return -1;
	va_start(ap, optname);
	switch (val->param_type)
	{
	case PT_YESNO :
	case PT_TRUEFALSE :
		d = va_arg(ap, int*);
		*d = val->val.bool_val;
		break;
	case PT_NUMERIC :
		d = va_arg(ap, int*);
		*d = val->val.num_val;
		break;
	case PT_STRING :
	case PT_FILENAME :
		s = va_arg(ap, char**);
		*s = strdup(val->val.str_val);
		break;
	case PT_NUMERIC_LIST :
		a = va_arg(ap, vector_t**);
		*a = vector_copy(val->val.vector_val);
		break;
	case PT_STRING_LIST :
	case PT_FILENAME_LIST :
		a = va_arg(ap, vector_t**);
		*a = vector_deep_copy(val->val.vector_val, (libcontain_copy_func_t)strdup);
		break;
	default :
		rc = -1;
	}
	va_end(ap);

	return rc; 
}
int libconf_setopt(libconf_t * handle, const char * optname, ...)
{
	va_list ap;
	libconf_opt_t * opt;
	int rc = 0;
	int d;
	char * s;
	vector_t * a;

	opt = (libconf_opt_t*)hash_get(handle->options, (void*)optname);
	if (opt == NULL)
		return -1;
	va_start(ap, optname);
	switch (opt->co_long_param_type)
	{
	case PT_YESNO :
	case PT_TRUEFALSE :
		d = va_arg(ap, int);
		hash_put(handle->option_hash, (void*)optname, (void*)d);
		break;
	case PT_NUMERIC :
		d = va_arg(ap, int);
		hash_put(handle->option_hash, (void*)optname, (void*)d);
		break;
	case PT_STRING :
	case PT_FILENAME :
		s = va_arg(ap, char*);
		hash_put(handle->option_hash, (void*)optname, strdup(s));
		break;
	case PT_NUMERIC_LIST :
		a = va_arg(ap, vector_t*);
		hash_put(handle->option_hash, (void*)optname, vector_copy(a));
		break;
	case PT_STRING_LIST :
	case PT_FILENAME_LIST :
		a = va_arg(ap, vector_t*);
		hash_put(handle->option_hash, (void*)optname, vector_deep_copy(a, (libcontain_copy_func_t)strdup));
		break;
	default :
		rc = -1;
	}
	va_end(ap);

	return rc; 
}

