%{
/* tokenize the preprocessor's input */
/* includes go here */
#include <stdlib.h>
#include <string.h>
#include <libcontain/stack.h>
#include "preprocp.h"
#include "preproc.h"

/* defines go gere */
#define YY_INPUT(buf, result, max_size) { result = preprocl_getc(buf, max_size); }

stack_t * include_stack = NULL;
char * curr_file = NULL;
extern FILE * dev_null;
%}
/* options go here */
/* %option debug */
%option noyywrap
%option yylineno
%option never-interactive
%option stack
%pointer

/* modes/states go here */
%x comment_state
%x quoted_txt_state
%x incl_state1
%x incl_state2
%x def_state1
%x def_state2
%x def_state3
%x undef_state
%x cond_state

/* class definitions go here */
STR		[^ \t\\\n[\"]+
FILENAME	[\.a-zA-Z0-9_][\.a-zA-Z0-9_/]+

/* rules come after the next line */
%%
<INITIAL,comment_state>"["		{ yy_push_state(comment_state); fwrite(" ", 1, 1, yyout); }
<INITIAL>\"		{ yy_push_state(quoted_txt_state); fwrite("\"", 1, 1, yyout); }
<INITIAL>"!include"	{ 
	yy_push_state(incl_state1); 
	if (yyout != dev_null)
		return T_INCLUDE; 
			}
<INITIAL>"!define"	{ yy_push_state(def_state1); }
<INITIAL>"!if"		{ yy_push_state(cond_state); return T_IF; }
<INITIAL>"!ifdef"	{ return T_IFDEF; }
<INITIAL>"!ifndef"      { return T_IFNDEF; }
<INITIAL>"!endif"       { return T_ENDIF; }
<INITIAL>"!else"	{ return T_ELSE; }
<INITIAL>"!undef"	{ yy_push_state(undef_state); }
<INITIAL>[^ \t\\\n[\"]+	{ yylval.str = strdup(yytext); return T_TOKEN; }
<INITIAL>[ \t]*		{ fwrite(yytext, yyleng, 1, yyout); }

<*>\\.			{ 
	switch (yytext[1])
	{
	case '[' :	fwrite("[", 1, 1, yyout); break;
	default :	fwrite(yytext, 2, 1, yyout); break;
	}
			}

<comment_state>"]"	{ yy_pop_state(); fwrite(" ", 1, 1, yyout); }
<comment_state>.	{ fwrite(" ", 1, 1, yyout); }

<quoted_txt_state>[^\"\\]* { fwrite(yytext, yyleng, 1, yyout); }
<quoted_txt_state>\"	{ yy_pop_state(); fwrite("\"", 1, 1, yyout); }

<incl_state1>[ \t]*	/* forget the white space */
<incl_state1>[\"<>]	{ 
	yy_push_state(incl_state2); 
	if (yyout != dev_null) 
		return yytext[0]; 
			}
<incl_state1>\n		{
	if (yyout != dev_null)
	{
		fprintf(stderr, "%s: %d: Error: expected filename\n", curr_file, yylineno - 1);
		exit(1);
	}
	yy_pop_state();
			}
<incl_state1>[\"<>\n]   {
	yy_pop_state();
	yy_pop_state();
			}
<incl_state2>[^\n\"<>]+	{ /* the include filename */
	if (yyout != dev_null)
	{
		yylval.str = strdup(yytext);
		return T_FILENAME;
	}
			}
<undef_state>[ \t]*     /* forget the white space */
<undef_state>[^ \t\n]+	{
	yy_pop_state();
	yylval.str = strdup(yytext);
	return T_UNDEF;
			}

<def_state1>[ \t]*	/* forget the white space */
<def_state1>[^ \t\n]+	{ /* the macro name */
	yy_push_state(def_state2);
	yylval.str = strdup(yytext);
	return T_DEFINE;
			}
<def_state2>\n          { 
	yy_pop_state(); 
	yy_pop_state(); 
	yylval.str = strdup("");
	return T_DEFINED;
			}
<def_state2>[ \t]*	{
		yy_push_state(def_state3);
	}
<def_state3>[^\n]*	{
	yy_pop_state();
	yy_pop_state();
	yy_pop_state();
	yylval.str = strdup(yytext);
	return T_DEFINED;
			}

<cond_state>[^\n]+	{ /* the expression */
	yy_pop_state();
	yylval.str = strdup(yytext);
	return T_EXPR;
			}

<<EOF>>		{
	preproc_inputfile_t * curr;

	if (include_stack == NULL)
	{
		yyterminate();
	}
	curr = stack_top(include_stack);
	stack_pop(include_stack);
	if (!curr)
		yyterminate();
	yy_delete_buffer(YY_CURRENT_BUFFER);
	yy_switch_to_buffer(curr->state);
	yylineno = curr->lineno;
	curr_file = curr->filename;
		}

<*>.	{ return yytext[0]; }
%%
/* C code goes here */
int preprocl_getc(char * buffer, size_t max_size)
{
	return read(fileno(yyin), buffer, max_size);
}

void preprocl_puts(const char * s)
{
	preproc_inputfile_t * curr = new_preproc_inputfile();
		
	if (include_stack == NULL)
		include_stack = new_stack();
	curr->filename = curr_file;
	curr->lineno = yylineno;
	curr->state = YY_CURRENT_BUFFER;
	stack_push(include_stack, curr);
	yy_switch_to_buffer(yy_scan_string(s));
}
