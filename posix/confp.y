/* Jail: Just Another Interpreted Language
 * Copyright (c) 2004, Ronald Landheer-Cieslak
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
%{
/* includes go here */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "../libconf.h"

/* defines go here */
/* we want the parser and the scanner to take a parameter */
#define YYPARSE_PARAM handle
#define YYLEX_PARAM handle
#define HANDLE ((libconf_t*)handle)
#define setval_prepare()										\
		HANDLE->temp = strdup(yylval.str)
#define setval(type)  																		\
	{																								\
		switch (type)																			\
		{																							\
		case PT_NUMERIC_LIST :																\
		case PT_STRING_LIST :																\
		case PT_FILENAME_LIST :																\
			{																						\
				libconf_optparam_t * param = hash_get(									\
					HANDLE->option_hash, HANDLE->temp);									\
				if (param != NULL)															\
				{																					\
					if (type == PT_NUMERIC_LIST)											\
						array_push_back(														\
							param->val.array_val, (void*)atoi(yylval.str));			\
					else																			\
						array_push_back(														\
							param->val.array_val, strdup(yylval.str));				\
					break;																		\
				}																					\
			}																						\
		default :																				\
			{																						\
				libconf_optparam_t * param = libconf_optparam_new(					\
					HANDLE->temp, type, yylval.str);										\
				hash_put(HANDLE->option_hash, HANDLE->temp, param);				\
			}																						\
		}																							\
	}

void emit_error(libconf_t * handle, const char * fmt, ...)
{
	va_list ap;
	
	switch (handle->default_error_action)
	{
	case DOE_ERROR :
		fprintf(stderr, "%s:%d: Error: ", handle->filename, handle->yylineno);
		break;
	case DOE_WARNING :
		fprintf(stderr, "%s:%d: Warning: ", handle->filename, handle->yylineno);
		break;
	default :
		return;
	}
	
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);	
	va_end(ap);
	
	fprintf(stderr, "\n");
	if (handle->default_error_action == DOE_ERROR)
		exit(1);
}

void yyerror(char * str)
{
	fprintf(stderr, "%s\n", str);
}

%}

%union {
	char * str;
	int num;
}

/* note, we want the parser to be re-entrant */
%pure_parser
/* note, we want a separate header files */
%defines
/* turn on parser debugging */
%debug

%token <str> T_UNKNOWN_OPTION T_TRUEFALSE T_YESNO T_NUMERIC T_STRING T_FILENAME
%token <str> T_NUMERIC_LIST T_STRING_LIST T_FILENAME_LIST
%token <str> T_TRUE T_YES T_NUM T_STR T_FILE T_LISTVAL
%token T_ENDOFLIST
%%

whole_file : /* empty */
	| config_def whole_file
	;

config_def /* not empty */
	: T_UNKNOWN_OPTION {
		emit_error(HANDLE, "unknown option: %s", yylval.str);
	}
	| T_TRUEFALSE { setval_prepare(); } ':' T_TRUE { setval(PT_TRUEFALSE); }
	| T_YESNO { setval_prepare(); } ':' T_YES { setval(PT_YESNO); }
	| T_NUMERIC { setval_prepare(); } ':' T_NUM { setval(PT_NUMERIC); }
	| T_STRING { setval_prepare(); } ':' T_STR { setval(PT_STRING); }
	| T_FILENAME { setval_prepare(); } ':' T_FILE { setval(PT_FILENAME); }
	| T_NUMERIC_LIST { 
		setval_prepare(); HANDLE->t_pt = PT_NUMERIC_LIST; 
	} ':' list_val
	| T_STRING_LIST {
		setval_prepare(); HANDLE->t_pt = PT_STRING_LIST;
	} ':' list_val
	| T_FILENAME_LIST {
		setval_prepare(); HANDLE->t_pt = PT_FILENAME_LIST; 
	} ':' list_val
	;
	
list_val /* not empty */
	: T_LISTVAL { setval(HANDLE->t_pt); } list_val
	| ':' T_LISTVAL { setval(HANDLE->t_pt); } list_val
	| T_ENDOFLIST
	;
%%
