#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
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
}

void free_preproc_inputfile(preproc_inputfile_t * preproc_inputfile)
{
	free(preproc_inputfile);
}

preproc_token_def_t * new_preproc_token_def(void)
{
	preproc_token_def_t * retval = (preproc_token_def_t*)malloc(sizeof(preproc_token_def_t));
	memset(retval, 0, sizeof(preproc_token_def_t));
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

