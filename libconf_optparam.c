#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "libconf.h"
#include "libreplace/catstr.h"

libconf_optparam_t * libconf_optparam_dup(libconf_optparam_t * param)
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

libconf_optparam_t ** libconf_optparams_dup(libconf_optparam_t ** params)
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
		retval->val.str_val = strdup(str);
		break;
	case PT_NUMERIC_LIST :
		retval->val.array_val = new_array(0);
		array_push_back(retval->val.array_val, (void*)atoi(str));
		break;
	case PT_STRING_LIST :
	case PT_FILENAME_LIST :
		retval->val.array_val = new_array(0);
		array_push_back(retval->val.array_val, strdup(str));
		break;
	default :
		free(retval);
		return NULL;
	}

	return retval;
}

