#ifndef _LIBCONF_LIBCONF_H
#define _LIBCONF_LIBCONF_H

#include <libcontain/hash.h>

typedef enum { NO, YES, OPTIONAL } libconf_take_parm_t;
typedef enum { NONE, YESNO, TRUEFALSE, NUMERIC, STRING } libconf_param_type_t;
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
} libconf_opt_t;

typedef struct _libconf_t 
{
	char * global_config_filename;
	char * local_config_filename;
	libconf_opt_t ** options;
	hash_t * option_hash;
} libconf_t;

/* initialize the library */
libconf_t * libconf_init(
	const char * global_config_filename,
	const char * local_config_filename,
	const libconf_opt_t ** options
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

#endif // _LIBCONF_LIBCONF_H
