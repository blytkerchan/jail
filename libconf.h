#ifndef _LIBCONF_LIBCONF_H
#define _LIBCONF_LIBCONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "libcontain/hash.h"

typedef enum { TP_NO, TP_YES, TP_OPTIONAL } libconf_take_parm_t;
typedef enum { PT_NONE, PT_YESNO, PT_TRUEFALSE, PT_NUMERIC, PT_STRING, PT_FILENAME } libconf_param_type_t;
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
	libconf_param_type_t param_type;
	union {
		int 	bool_val;
		char * 	str_val;
		int 	num_val;
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

	/* These are for the POSIX implementation */
	void * Lexer;
	void * yyin;
	void * yyout;
	char * filename;
	char * yylineno;
	void * temp;
} libconf_t;

/* initialize the library */
libconf_t * libconf_init(
	const char * global_config_filename,
	const char * local_config_filename,
	const libconf_opt_t ** options,
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

libconf_optparam_t * libconf_optparam_new(libconf_param_type_t libconf_param_type, char * str);

#ifdef __cplusplus
}
#endif

#endif // _LIBCONF_LIBCONF_H
