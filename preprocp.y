%{
/* parse the Jail preprocessor input */
/* includes go here */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <libcontain/hash.h>
#include <libcontain/stack.h>
#include "preproc.h"

/* defines go here */
#define YYDEBUG 1

hash_t * token_hash = NULL;
stack_t * outfile_stack = NULL;
FILE * dev_null = NULL;
preproc_token_def_t * curr;

void yyerror(char * s)
{
	fprintf(stderr, "%s\n", s);
}

%}

%union {
	char * str;
}

%debug

%token <str> T_TOKEN T_DEFINE T_DEFINED T_EXPR T_UNDEF T_FILENAME
%token T_IFDEF T_IF T_IFNDEF T_ENDIF T_ELSE T_INCLUDE
%%

whole_file : /* empty*/
	| token_def whole_file
	| macro_def whole_file
	| macro_undef whole_file
	| macro_cond whole_file
	| include_directive whole_file
	;

token_def : T_TOKEN {
		if (token_hash)
		{
			if ((curr = hash_get(token_hash, yylval.str)) != NULL)
			{
				preprocl_puts(curr->val);
			}
			else
				fwrite(yylval.str, strlen(yylval.str), 1, yyout);
		}
		else
			fwrite(yylval.str, strlen(yylval.str), 1, yyout);
	}
	;

macro_undef : T_UNDEF {
	if (yyout != dev_null)
	{
		do
		{
			if (!token_hash)
			{
				fprintf(stderr, "%s: %d: Warning: trying to undefine an undefined macro\n", curr_file, yylineno);
				break;
			}
			curr = hash_get(token_hash, yylval.str);
			if (curr == NULL)
			{
				fprintf(stderr, "%s: %d: Warning: trying to undefine an undefined macro\n", curr_file, yylineno);
				break;
			}
			hash_remove(token_hash, yylval.str);
		} while (0);
	}
	}
	;
macro_def : T_DEFINE {
		if (yyout != dev_null)
		{
			if (!token_hash)
			{
				token_hash = hash_new(STRING_HASH, 0, NULL, NULL);
			}
			curr = hash_get(token_hash, yylval.str);
			if (curr != NULL)
			{
				fprintf(stderr, "%s: %d: Warning: redefinition of %s\n", curr_file ? curr_file : "<stdin>", yylineno, yylval.str);
				fprintf(stderr, "%s: %d: Warning: previous definition was here\n", curr->inputfile->filename, curr->inputfile->lineno);
			}
			else
			{
				curr = new_preproc_token_def();
				curr->inputfile = new_preproc_inputfile();
				curr->inputfile->filename = curr_file;
				curr->inputfile->lineno = yylineno;
			}
			curr->name = yylval.str;
		}
	} 	T_DEFINED {
		if (yyout != dev_null) 
		{
			curr->val = strdup(yylval.str);
			hash_put(token_hash, curr->name, curr);
		}
	}
	;

macro_cond : T_IFDEF T_TOKEN {
		int rc = 1;
		
		if (!token_hash)
			rc = 0;
		else if (hash_get(token_hash, yylval.str) == NULL)
			rc = 0;
		if (!outfile_stack)
			outfile_stack = new_stack();
		stack_push(outfile_stack, yyout);
		if (!rc)
		{
			if (!dev_null)
			{
				dev_null = fopen("/dev/null", "ab");
				assert(dev_null != NULL);
			}
			yyout = dev_null;
		}
	} whole_file macro_cond_end
	| T_IFNDEF T_TOKEN {
		int rc = 1;
		
		if (!token_hash)
			rc = 0;
		else if (hash_get(token_hash, yylval.str) == NULL)
			rc = 0;
		if (!outfile_stack)
			outfile_stack = new_stack();
		stack_push(outfile_stack, yyout);
		if (rc)
		{
			if (!dev_null)
			{
				dev_null = fopen("/dev/null", "ab");
				assert(dev_null != NULL);
			}
			yyout = dev_null;
		}
	} whole_file macro_cond_end
	| T_IF T_EXPR {} whole_file macro_cond_end
	;

macro_cond_end : T_ENDIF {
		yyout = stack_top(outfile_stack);
		stack_pop(outfile_stack);
	}
	| T_ELSE {
		if (stack_top(outfile_stack) != dev_null)
		{
			if (yyout == dev_null)
				yyout = stack_top(outfile_stack);
			else 
			{
				if (dev_null == NULL)
					dev_null = fopen("/dev/null", "ab");
				yyout = dev_null;
			}
		}
	} whole_file T_ENDIF {
		yyout = stack_top(outfile_stack);
		stack_pop(outfile_stack);
	}
	;

include_directive : T_INCLUDE '<' abs_filename '>'
	| T_INCLUDE '\"' rel_filename '\"'
	;
abs_filename : T_FILENAME {
	const char * filename = resolve_abs_filename(yylval.str);

	if (filename == NULL)
	{
		fprintf(stderr, "%s: %d: Error: file not found: %s\n", curr_file, yylineno, yylval.str);
		exit(1);
	}
	if (preprocl_putf(filename) != 0)
	{
		fprintf(stderr, "%s: %d: Error: failed to read from %s\n", curr_file, yylineno, filename);
		exit(1);
	}
	}
	;
rel_filename : T_FILENAME {
	const char * filename = resolve_rel_filename(yylval.str);

	if (filename == NULL)
	{
		fprintf(stderr, "%s: %d: Error: file not found: %s\n", curr_file, yylineno, yylval.str);
		exit(1);
	}
	if (preprocl_putf(filename) != 0)
	{
		fprintf(stderr, "%s: %d: Error: failed to read from %s\n", curr_file, yylineno, filename);
		exit(1);
	}
	}
	;

%%
int main(int argc, char ** argv)
{
//	yydebug = 1;
	if (argc > 1)
	{
		curr_file = strdup(argv[1]);
		yyin = fopen(argv[1], "r");
	}
	yyparse();
}

