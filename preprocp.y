%{
/* parse the Jail preprocessor input */
/* includes go here */
#include <assert.h>
#include <stdio.h>
#include <libcontain/hash.h>
#include <libcontain/stack.h>

/* defines go here */
#define YYDEBUG 1

hash_t * token_hash = NULL;
int yylex(void);
extern int yylineno;
extern FILE * yyin;
extern FILE * yyout;
stack_t * outfile_stack = NULL;
FILE * dev_null = NULL;
char * curr;

void yyerror(char * s)
{
	fprintf(stderr, "%s\n", s);
}

%}

%union {
	char * str;
}

%debug

%token <str> T_TOKEN T_DEFINE T_DEFINED T_EXPR
%token T_IFDEF T_IF T_IFNDEF T_ENDIF
%%

whole_file : /* empty*/
	| token_def whole_file
	| macro_def whole_file
	| macro_cond whole_file
	;

token_def : T_TOKEN {
		if (token_hash)
		{
			if ((curr = hash_get(token_hash, yylval.str)) != NULL)
				fwrite(curr, strlen(curr), 1, yyout);
			else
				fwrite(yylval.str, strlen(yylval.str), 1, yyout);
		}
		else
			fwrite(yylval.str, strlen(yylval.str), 1, yyout);
	}
	;

macro_def : T_DEFINE {
		if (yyout != dev_null)
		{
			if (!token_hash)
			{
				token_hash = new_hash(STRING_HASH, NULL, NULL);
			}
			curr = hash_get(token_hash, yylval.str);
			if (curr != NULL)
				fprintf(stderr, "Warning: redefinition of %s\n", yylval.str);
			curr = yylval.str;
		}
	} 	T_DEFINED {
		if (yyout != dev_null) hash_put(token_hash, curr, yylval.str);
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
	} whole_file T_ENDIF {
		yyout = stack_top(outfile_stack);
		stack_pop(outfile_stack);
	}
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
	} whole_file T_ENDIF {
		yyout = stack_top(outfile_stack);
		stack_pop(outfile_stack);
	}
	| T_IF T_EXPR {} whole_file T_ENDIF {}
	;
	
%%
int main(int argc, char ** argv)
{
//	yydebug = 1;
	if (argc > 1)
		yyin = fopen(argv[1], "r");
	yyparse();
}

