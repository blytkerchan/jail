#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "libconf.h"
#include "libreplace/catstr.h"

static const libconf_opt_t default_options[] = {
	{ "local-config", 'C', TP_YES, PT_FILENAME, "config", TP_YES, PT_FILENAME, "expects a filename, which should point to a local configuration file to use in stead of the default", DOE_ERROR },
	{ "global-config", 'G', TP_YES, PT_YESNO, "global-config", TP_YES, PT_YESNO, "expects \"yes\" or \"no\". If it's \"no\" the global configuration will not be parsed. If it is something other than \"yes\" or \"no\", a warning will be emitted.", DOE_WARNING },
	{ "verbose", 'v', TP_OPTIONAL, PT_YESNO, "verbose", TP_OPTIONAL, PT_YESNO, "be versbose - optionally takes an argument \"yes\" or \"no\".", DOE_NOTHING },
	{ "help", 'h', TP_NO, PT_NONE, "help", TP_NO, PT_NONE, "display a help screen", DOE_NOTHING },
	{ NULL, 0, TP_NO, PT_NONE, NULL, TP_NO, PT_NONE, NULL, DOE_NOTHING }
}; 

static libconf_opt_t ** libconf_optdefaults(void)
{
	libconf_opt_t ** retval = NULL;
	size_t n, i;

	for (n = 0; default_options[i].co_name; n++);
	retval = (libconf_opt_t**)malloc(sizeof(libconf_opt_t) * (n + 1));
	retval[n] = NULL;
	for (i = 0; i < n; i++);
	{
		retval[i] = (libconf_opt_t*)malloc(sizeof(libconf_opt_t));
      retval[i]->co_name = strdup(default_options[i].co_name);

		retval[i]->co_short_opt = default_options[i].co_short_opt;
		retval[i]->co_short_takes_param = default_options[i].co_short_takes_param;
		retval[i]->co_short_param_type = default_options[i].co_short_param_type;

		retval[i]->co_long_opt = strdup(default_options[i].co_long_opt);
		retval[i]->co_long_takes_param = default_options[i].co_long_takes_param;
		retval[i]->co_long_param_type = default_options[i].co_long_param_type;

		retval[i]->helptext = strdup(default_options[i].helptext);
	}

	return retval;
}

static libconf_opt_t ** libconf_optdup(const libconf_opt_t ** options)
{
	libconf_opt_t ** retval;
	size_t i, n;

	if (options)
		for (n = 0; options[n]; n++) ;
	else
		n = 0;
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

static libconf_opt_t ** libconf_optcat(libconf_opt_t ** options1, const libconf_opt_t ** options2)
{
	libconf_opt_t ** opt2;
	size_t n1, n2, i;

	if (options1)
		for (n1 = 0; options1[n1]; n1++);
	else
		n1 = 0;
	if (options2)
		for (n2 = 0; options2[n2]; n2++);
	else
		n2 = 0;
	options1 = (libconf_opt_t**)realloc(options1, sizeof(libconf_opt_t*) * (n1 + n2 + 1));
	opt2 = libconf_optdup(options2);
	for (i = n1; i < n1 + n2; i++)
		options1[i] = opt2[i - n1];
	free(opt2);

	return options1;
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

libconf_t * libconf_init(
	const char * global_config_filename,
	const char * local_config_filename,
	const libconf_opt_t ** options,
	int argc, const char ** argv)
{
	int i;
	libconf_t * retval = (libconf_t *)malloc(sizeof(libconf_t));
	const libconf_opt_t ** temp = (const libconf_opt_t**)libconf_optdefaults();
	memset(retval, 0, sizeof(libconf_t));

	retval->global_config_filename = strdup(global_config_filename);
	retval->local_config_filename = strdup(local_config_filename);
	retval->options = libconf_optdup(options);
	retval->options = libconf_optcat(retval->options, temp);
	libconf_opts_free((libconf_opt_t**)temp);
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
	libconf_opts_free(handle->options);
	delete_hash(handle->option_hash);
	delete_hash(handle->tmp_hash);
	for (i = 0; i < handle->argc; i++)
		free(handle->argv[i]);
	free(handle->argv);
	free(handle);
}

static libconf_optparam_t * libconf_optparam_new(
	libconf_param_type_t param_type,
	char * str)
{
	libconf_optparam_t * retval = (libconf_optparam_t*)malloc(sizeof(libconf_optparam_t));
	memset(retval, 0, sizeof(libconf_optparam_t));
	
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
		retval->val.str_val = strdup(str);
		break;
	default :
		free(retval);
		return NULL;
	}

	return retval;
}

typedef enum { 
	ET_NONE, ET_EXPECTED_PARAM_TP_NOT_FOUND, ET_PARAM_MALFORMED, ET_EXPECTED_OPTION_TP_NOT_FOUND
} libconf_phase1_error_t;

/* read the command-line options and set the global_config_filename and 
 * local_config_filename fields accordingly */
int libconf_phase1(libconf_t * handle)
{
	int i;
	int argc = handle->argc;
	char ** argv = handle->argv;
	libconf_phase1_error_t have_error;
	libconf_optparam_t * param;
	libconf_do_on_error_t error_action = DOE_ERROR;
	int have_param;
	char * param_name;
	char * tmp_str;

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
				for (i = 0; handle->options[i]; i++)
				{
					if (handle->options[i]->co_long_opt)
					{
						if (strcmp(argv[0] + 2, handle->options[i]->co_long_opt) == 0)
						{ /* we've found the option */
							param_name = handle->options[i]->co_name;
							error_action = handle->options[i]->do_on_error;
							if (handle->options[i]->co_long_takes_param != TP_NO)
							{
								if (argc > 1 && argv[1][0] && argv[1][0] != '-')
								{ /* we have our option's param */
									param = libconf_optparam_new(handle->options[i]->co_long_param_type, argv[1]);
									if (param == NULL)
										have_error = ET_PARAM_MALFORMED;
									else if (param->have_error)
										have_error = ET_PARAM_MALFORMED;
									have_param = 1;
								}
								else if (handle->options[i]->co_long_takes_param == TP_YES)
								{ /* we should have had a param, but we don't */
									have_error = ET_EXPECTED_PARAM_TP_NOT_FOUND;
								}
							}
						}
					}
				}
				hash_put(handle->tmp_hash, argv[0], param);
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
				param = libconf_optparam_new(PT_STRING, tmp_str);
				free(tmp_str);
				hash_put(handle->tmp_hash, "-", param);
				break;
			default : /* we have a short option */
				for (i = 0; handle->options[i]; i++)
				{
					if (handle->options[i]->co_short_opt == argv[0][1])
					{	/* we have our option */
						param_name = handle->options[i]->co_name;
						error_action = handle->options[i]->do_on_error;
						if (handle->options[i]->co_short_takes_param != TP_NO)
						{
							if (argv[0][2])
							{ /* we have a parameter directly following */
								param = libconf_optparam_new(handle->options[i]->co_short_param_type, argv[0] + 2);
								if (param == NULL)
									have_error = ET_PARAM_MALFORMED;
								else if (param->have_error)
									have_error = ET_PARAM_MALFORMED;
								have_param = 1;
							}
							else if (argc > 1 && argv[1][0] && argv[1][0] != '-' )
							{	/* we have a parameter */
								param = libconf_optparam_new(handle->options[i]->co_short_param_type, argv[1]);
								if (param == NULL)
									have_error = ET_PARAM_MALFORMED;
								else if (param->have_error)
									have_error = ET_PARAM_MALFORMED;
								have_param = 1;
							}
							else if (handle->options[i]->co_short_takes_param == TP_YES)
							{	/* we should have had a param, but we don't */
								have_error = ET_EXPECTED_PARAM_TP_NOT_FOUND;
							}
						}
					}
				}
				hash_put(handle->tmp_hash, argv[0], param);
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

int libconf_phase2(libconf_t * handle) { return -1; }
int libconf_phase3(libconf_t * handle) { return -1; }
int libconf_phase4(libconf_t * handle) { return -1; }
int libconf_phase5(libconf_t * handle) { return -1; }

int libconf_getopt(libconf_t * handle, const char * optname, ...){ return -1; }
int libconf_setopt(libconf_t * handle, const char * optname, ...){ return -1; }

