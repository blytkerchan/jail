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
#ifndef _LIBCONF_LIBCONF_H
#define _LIBCONF_LIBCONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "libcontain/hash.h"
#include "libcontain/array.h"

typedef enum { TP_NO, TP_YES, TP_OPTIONAL } libconf_take_parm_t;
typedef enum { 
	PT_NONE, 
	PT_YESNO, 
	PT_TRUEFALSE, 
	PT_NUMERIC, 
	PT_STRING, 
	PT_FILENAME,
	PT_NUMERIC_LIST,
	PT_STRING_LIST,
	PT_FILENAME_LIST
} libconf_param_type_t;
typedef enum { DOE_NOTHING, DOE_WARNING, DOE_ERROR } libconf_do_on_error_t;
typedef struct _libconf_opt_t
{
	char * co_name;
	/* the short version of the option*/
	char co_short_opt;
	libconf_take_parm_t co_short_takes_param;
	libconf_param_type_t co_short_param_type;
	/* the long version of the option */
	char * co_long_opt;
	libconf_take_parm_t co_long_takes_param;
	libconf_param_type_t co_long_param_type;
	
	char * helptext;
	libconf_do_on_error_t do_on_error;
} libconf_opt_t;

typedef struct _libconf_optparam_t
{
	char * name;
	libconf_param_type_t param_type;
	union {
		int 			bool_val;
		char * 		str_val;
		int 			num_val;
		array_t * 	array_val;
	} val;
	int have_error;
} libconf_optparam_t;

typedef struct _libconf_t 
{
	char * global_config_filename;
	char * local_config_filename;
	hash_t * options;
	hash_t * option_hash;
	hash_t * tmp_hash;
	int argc;
	char ** argv;
	char * argv0;
	libconf_do_on_error_t default_error_action;
	libconf_optparam_t ** defaults;

	/* These are for the POSIX implementation */
	void * Lexer;
	void * yyin;
	void * yyout;
	char * filename;
	int yylineno;
	void * temp;
	libconf_param_type_t t_pt;
} libconf_t;

/* initialize the library */
libconf_t * libconf_init(
	const char * global_config_filename,
	const char * local_config_filename,
	const libconf_opt_t ** options,
	const libconf_optparam_t ** defaults,
	int argc, const char ** argv
		);
void libconf_fini(libconf_t * handle);

int libconf_phase1(libconf_t * handle);
int libconf_phase2(libconf_t * handle);
int libconf_phase3(libconf_t * handle);
int libconf_phase4(libconf_t * handle);
int libconf_phase5(libconf_t * handle);

int libconf_getopt(libconf_t * handle, const char * optname, ...);
int libconf_setopt(libconf_t * handle, const char * optname, ...);

void libconf_opts_free(libconf_opt_t ** opts);
void libconf_opt_free(libconf_opt_t * opt);
libconf_opt_t ** libconf_optdup(const libconf_opt_t ** options);
libconf_opt_t ** libconf_defaultopts(void);
libconf_opt_t ** libconf_optconst(libconf_opt_t * options);

libconf_optparam_t * libconf_optparam_new(
	char * name,
	libconf_param_type_t libconf_param_type, 
	char * str);
libconf_optparam_t * libconf_optparam_dup(libconf_optparam_t * param);
libconf_optparam_t ** libconf_optparams_dup(libconf_optparam_t ** params);
void libconf_optparam_free(libconf_optparam_t * param);
void libconf_optparams_free(libconf_optparam_t ** params);

#ifdef __cplusplus
}
#endif

#endif // _LIBCONF_LIBCONF_H
