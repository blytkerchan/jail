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
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if HAVE_LIBGEN_H
#	include <libgen.h>
#endif
#include "preproc.h"
#include "libreplace/macros.h"

static const char * include_search_path[] = {
	"/usr/share/justice/include",
	"/usr/justice/include",
	"/usr/local/justice/include",
	NULL
};

preproc_inputfile_t * new_preproc_inputfile(void)
{
	preproc_inputfile_t * retval = (preproc_inputfile_t*)malloc(sizeof(preproc_inputfile_t));
	memset(retval, 0, sizeof(preproc_inputfile_t));

	return retval;
}

void free_preproc_inputfile(preproc_inputfile_t * preproc_inputfile)
{
	free(preproc_inputfile);
}

preproc_token_def_t * new_preproc_token_def(void)
{
	preproc_token_def_t * retval = (preproc_token_def_t*)malloc(sizeof(preproc_token_def_t));
	memset(retval, 0, sizeof(preproc_token_def_t));

	return retval;
}

void free_preproc_token_def(preproc_token_def_t * preproc_token_def)
{
	free(preproc_token_def);
}

const char * resolve_abs_filename(const char * filename)
{
	int i;
	static char buffer[FILENAME_MAX];
	struct stat stat_info;

	for (i = 0; include_search_path[i]; i++)
	{
		strcpy(buffer, include_search_path[i]);
		cat_slash(buffer);
		strcat(buffer, filename);
		if (stat(buffer, &stat_info) == 0)
		{
			return buffer;
		}
	}
	return NULL;
}

const char * resolve_rel_filename(const char * filename)
{
	static char buffer[FILENAME_MAX];
	struct stat stat_info;
	
	strcpy(buffer, dirname(curr_file));
	cat_slash(buffer);
	strcat(buffer, filename);
	if (stat(buffer, &stat_info) == 0)
	{
		return buffer;
	}
	
	return resolve_abs_filename(filename);
}

