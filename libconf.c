#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "libconf.h"

static libconf_opt_t ** libconf_optdup(const libconf_opt_t ** options)
{
	libconf_opt_t ** retval;
	size_t i, n;

	for (n = 0; options[n]; n++) ;
	retval = (libconf_opt_t **)malloc((n + 1) * sizeof(libconf_opt_t *));
	retval[n] = NULL;
	for (i = 0; i < n; i++)
	{
		retval[i] = (libconf_opt_t *)malloc(sizeof(libconf_t));
		retval[i]->co_name = strdup(options[i]->co_name);
		
		retval[i]->co_short_opt = options[i]->co_short_opt;
		retval[i]->co_short_takes_param = options[i]->co_short_takes_param;
		retval[i]->co_short_param_type = options[i]->co_short_param_type;
		
		retval[i]->co_long_opt = strdup(options[i]->co_long_opt);
		retval[i]->co_long_takes_param = options[i]->co_long_takes_param;
		retval[i]->co_long_param_type = options[i]->co_long_param_type;
		
		retval[i]->helptext = strdup(options[i]->helptext);
	}

	return retval;
}

void libconf_opt_free(libconf_opt_t * opt)
{
	free(opt->co_name);
	if (opt->co_long_opt) free(opt->co_long_opt);
	if (opt->helptext) free(opt->helptext);
	free(opt);
}

void libconf_opts_free(libconf_opt_t ** opts)
{
	libconf_opt_t * curr;
	int i = 0;

	while ((curr = opts[i++]) != NULL)
		libconf_opt_free(curr);
	free(opts);
}

/* initialize the library */
libconf_t * libconf_init(
	const char * global_config_filename,
	const char * local_config_filename,
	const libconf_opt_t ** options)
{
	libconf_t * retval = (libconf_t *)malloc(sizeof(libconf_t));
	memset(retval, 0, sizeof(libconf_t));

	retval->global_config_filename = strdup(global_config_filename);
	retval->local_config_filename = strdup(local_config_filename);
	retval->options = libconf_optdup(options);
	retval->option_hash = new_hash(STRING_HASH, NULL, NULL);

	return retval;
}
void libconf_fini(libconf_t * handle)
{
	free(handle->global_config_filename);
	free(handle->local_config_filename);
	libconf_opts_free(handle->options);
	delete_hash(handle->option_hash);
	free(handle);
}

int libconf_phase1(libconf_t * handle);
int libconf_phase2(libconf_t * handle);
int libconf_phase3(libconf_t * handle);
int libconf_phase4(libconf_t * handle);
int libconf_phase5(libconf_t * handle);

int libconf_getopt(libconf_t * handle, const char * optname, ...);
int libconf_setopt(libconf_t * handle, const char * optname, ...);

