#ifndef _PREPROCESSOR_PREPROC_H
#define _PREPROCESSOR_PREPROC_H

/* data type definitions */
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

/* variables found in preproc.c */
extern const char * include_search_path[];

/* variables found in preprocl.l */
extern char * curr_file;
extern int yylineno;
extern FILE * yyin;
extern FILE * yyout;

/* variables found in preprocp.y */
extern FILE * dev_null;

/* functions in preproc.c */
preproc_inputfile_t * new_preproc_inputfile(void);
void free_preproc_inputfile(preproc_inputfile_t * preproc_inputfile);

preproc_token_def_t * new_preproc_token_def(void);
void free_preproc_token_def(preproc_token_def_t * preproc_token_def);

const char * resolve_abs_filename(const char * filename);
const char * resolve_rel_filename(const char * filename);

/* found in preprocl.l */
void preprocl_putc(char c);
int yylex(void);

#endif // _PREPROCESSOR_PREPROC_H
