#include "preproc.h"

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

