%{
/* tokenize the preprocessor's input */
/* includes go here */
#include <stdlib.h>
#include <string.h>
#include <libcontain/stack.h>
#include "preprocp.h"

/* defines go gere */
#define YY_INPUT(buf, result, max_size) { result = preprocl_getc(buf, max_size); }

stack_t * include_stack = NULL;
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
%x incl_state
%x def_state
%x cond_state

/* class definitions go here */
STR		[ \t\\\n[\"]+
FILENAME	[\.a-zA-Z0-9_][\.a-zA-Z0-9_/]+

/* rules come after the next line */
%%
<INITIAL,comment_state>"["		{ yy_push_state(comment_state); fwrite(" ", 1, 1, yyout); }
<INITIAL>\"		{ yy_push_state(quoted_txt_state); fwrite("\"", 1, 1, yyout); }
<INITIAL>"!include"	{ yy_push_state(incl_state); }
<INITIAL>"!define"	{ yy_push_state(def_state); }
<INITIAL>"!if"		{ yy_push_state(cond_state); return T_IF; }
<INITIAL>"!ifdef"	{ return T_IFDEF; }
<INITIAL>"!ifndef"      { return T_IFNDEF; }
<INITIAL>"!endif"       { return T_ENDIF; }
<INITIAL>[^ \t\\\n[\"]+	{ yylval.str = strdup(yytext); return T_TOKEN; }
<INITIAL>[ \t]*		{ fwrite(yytext, yyleng, 1, yyout); }

<*>\\.			{ 
	switch (yytext[1])
	{
	case '[' :	fwrite("[", 1, 1, yyout); break;
	default :	fwrite(yytext, 2, 1, yyout); break;
	}
			}
<*>\n			{ yylineno++; ECHO; }

<comment_state>"]"	{ yy_pop_state(); fwrite(" ", 1, 1, yyout); }
<comment_state>.	{ fwrite(" ", 1, 1, yyout); }

<quoted_txt_state>[^\"\\]* { fwrite(yytext, yyleng, 1, yyout); }
<quoted_txt_state>\"	{ yy_pop_state(); fwrite("\"", 1, 1, yyout); }

<incl_state>[ \t]*	/* forget the white space */
<incl_state>[^ \t\n]+	{ /* the include filename */
	if (include_stack == NULL)
		include_stack = new_stack();
	stack_push(include_stack, YY_CURRENT_BUFFER);
	yyin = fopen(yytext, "r");
	if (!yyin)
		error("...");
	yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
	yy_pop_state();
			}

<def_state>[ \t]*	/* forget the white space */
<def_state>[^ \t\n]+	{ /* the macro name */
	yy_pop_state();
	yylval.str = strdup(yytext);
	return T_DEFINE;
			}

<cond_state>[^\n]+	{ /* the expression */
	yy_pop_state();
	yylval.str = strdup(yytext);
	return T_EXPR;
			}

<<EOF>>		{
	YY_BUFFER_STATE curr;

	if (include_stack == NULL)
	{
		yyterminate();
	}
	curr = stack_top(include_stack);
	stack_pop(include_stack);
	if (!curr)
		yyterminate();
	yy_delete_buffer(YY_CURRENT_BUFFER);
	yy_switch_to_buffer(curr);
		}
%%
/* C code goes here */
int preprocl_getc(char * buffer, size_t max_size)
{
	return read(fileno(yyin), buffer, max_size);
}


