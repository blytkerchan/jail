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
#include "libreplace/memdup.h"

static const libconf_opt_t default_options[] = {
	{ "local-config", 'C', TP_YES, PT_FILENAME, "config", TP_YES, PT_FILENAME, "expects a filename, which should point to a local configuration file to use in stead of the default", DOE_ERROR },
	{ "global-config", 'G', TP_YES, PT_YESNO, "global-config", TP_YES, PT_YESNO, "expects \"yes\" or \"no\". If it's \"no\" the global configuration will not be parsed. If it is something other than \"yes\" or \"no\", a warning will be emitted.", DOE_WARNING },
	{ "verbose", 'v', TP_OPTIONAL, PT_YESNO, "verbose", TP_OPTIONAL, PT_YESNO, "be versbose - optionally takes an argument \"yes\" or \"no\".", DOE_NOTHING },
	{ "help", 'h', TP_NO, PT_NONE, "help", TP_NO, PT_NONE, "display a help screen", DOE_NOTHING },
	{ NULL, 0, TP_NO, PT_NONE, NULL, TP_NO, PT_NONE, NULL, DOE_NOTHING }
}; 

libconf_opt_t ** libconf_optconst(const libconf_opt_t * options)
{
	int i, n;
	libconf_opt_t ** retval;
	
	for (n = 0; options[n].co_name; n++);
	retval = (libconf_opt_t**)calloc(n + 1, sizeof(libconf_opt_t*));
	for (i = 0; i < n; i++)
		retval[i] = memdup(&(options[i]), sizeof(libconf_opt_t));

	return retval;
}

libconf_opt_t ** libconf_defaultopts(void)
{
	return libconf_optconst(default_options);
}

libconf_opt_t ** libconf_optdup(const libconf_opt_t ** options)
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

/*
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
*/

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


