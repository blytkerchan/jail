#ifndef _PREPROCESSOR_PREPROC_H
#define _PREPROCESSOR_PREPROC_H

typedef struct _preproc_inputfile_t
{
	char * 				filename;
	unsigned int 		lineno;
	void * 				state;
} preproc_inputfile_t;

typedef struct _preproc_token_def_t
{
	preproc_inputfile_t * inputfile;
	char * name;
	char * val;
} preproc_token_def_t;

preproc_inputfile_t * new_preproc_inputfile(void);
void free_preproc_inputfile(preproc_inputfile_t * preproc_inputfile);

preproc_token_def_t * new_preproc_token_def(void);
void free_preproc_token_def(preproc_token_def_t * preproc_token_def);

char * resolve_abs_filename(const char * filename);
char * resolve_rel_filename(const char * filename);

/* found in preprocl.l */
void preprocl_putc(char c);

#endif // _PREPROCESSOR_PREPROC_H
