#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "libconf.h"
#include <libconf_config.h>
#include "libreplace/catstr.h"

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

	retval->global_config_filename = strdup(global_config_filename);
	retval->local_config_filename = strdup(local_config_filename);
	retval->options = new_hash(STRING_HASH, NULL, NULL);
	t_options = libconf_defaultopts();
	for (i = 0; t_options[i]; i++)
	{
		hash_put(retval->options, t_options[i]->co_name, t_options[i]);
	}
	t_options = libconf_optdup(options);
	for (i = 0; t_options[i]; i++)
	{
		hash_put(retval->options, t_options[i]->co_name, t_options[i]);
	}
	free(t_options);
	retval->option_hash = new_hash(STRING_HASH, NULL, NULL);
	retval->tmp_hash = new_hash(STRING_HASH, NULL, NULL);
	retval->argc = argc;
	retval->argv = (char**)malloc((argc + 1) * sizeof(char*));
	for (i = 0; i < argc; i++)
		retval->argv[i] = strdup(argv[i]);

	return retval;
}
void libconf_fini(libconf_t * handle)
{
	int i;

	free(handle->global_config_filename);
	free(handle->local_config_filename);
	delete_hash(handle->options);
	delete_hash(handle->option_hash);
	delete_hash(handle->tmp_hash);
	for (i = 0; i < handle->argc; i++)
		free(handle->argv[i]);
	free(handle->argv);
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
	while (argc)
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
							param = libconf_optparam_new(option->co_name, option->co_long_param_type, argv[1]);
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
					hash_put(handle->tmp_hash, argv[0], param);
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
				hash_put(handle->tmp_hash, "-", param);
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
							param = libconf_optparam_new(option->co_name, option->co_short_param_type, argv[0] + 2);
							if (param == NULL)
								have_error = ET_PARAM_MALFORMED;
							else if (param->have_error)
								have_error = ET_PARAM_MALFORMED;
							have_param = 1;
						}
						else if (argc > 1 && argv[1][0] && argv[1][0] != '-' )
						{	/* we have a parameter */
							param = libconf_optparam_new(option->co_name, option->co_short_param_type, argv[1]);
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
					hash_put(handle->tmp_hash, argv[0], param);
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
			argv++; argc--;
		argv++; argc--;
	}

	return 0;
}

/* set the global defaults */
int libconf_phase2(libconf_t * handle)
{
	int i;
	
	for (i = 0; handle->defaults[i]; i++)
		hash_put(handle->option_hash, handle->defaults[i]->name, handle->defaults[i]);
	
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
	hash_put(handle->option_hash, key, val);
}
int libconf_phase5(libconf_t * handle) 
{
	hash_foreach(handle->tmp_hash, libconf_phase5_helper, handle);
}

int libconf_getopt(libconf_t * handle, const char * optname, ...){ return -1; }
int libconf_setopt(libconf_t * handle, const char * optname, ...){ return -1; }

