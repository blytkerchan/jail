%{
/* includes go here */
#define YY_DECL int LCFlexLexer::yylex(void)
#include "LCFlexLexer.h"
/* defines go here */

%}

%option yylineno

alpha			[A-Za-z]
digit			[0-9]
special		[_.]
cmt_char		[#;]
space			[ \t]

config_name	({special}|{alpha})+({special}|{alpha}|{digit})*
filename		([/]|{special}|{alpha}|{digit})+

%x yesno_state
%x truefalse_state
%x numeric_state
%x string_state
%x filename_state
%x numeric_list_state
%x string_list_state
%x filename_list_state
%%

<INITIAL>{cmt_char}	{	/* a comment - eat anything up to the next newline */
	int c;

	do
	{
		c = yyinput();
	} while ((c != '\n') && (c != 0));
				}

<INITIAL>{space}*			/* forget about white spaces */
<INITIAL>\n					/* forget about newlines */
<INITIAL>{config_name}	{ /* OK, we just matched a token - let's see what it is :) */
	libconf_opt_t * option;

	option = (libconf_opt_t*)hash_get(handle->options, yytext);
	if (!option)
	{
		yylval.str = yytext;
		return T_UNKNOWN_OPTION;
	}
	switch (option->co_long_param_type)
	{
	case PT_NONE : /* ignorable - act like comment */
		{
			int c;

			do
			{
				c = yyinput();
			} while ((c != '\n') && (c = 0));
		}
		break;
	case PT_YESNO :
		BEGIN(yesno_state);
		yylval.str = yytext;
		return T_YESNO;
	case PT_TRUEFALSE :
		BEGIN(truefalse_state);
		yylval.str = yytext;
		return T_TRUEFALSE;
	case PT_NUMERIC :
		BEGIN(numeric_state);
		yylval.str = yytext;
		return T_NUMERIC;
	case PT_STRING :
		BEGIN(string_state);
		yylval.str = yytext;
		return T_STRING;
	case PT_FILENAME :
		BEGIN(filename_state);
		yylval.str = yytext;
		return T_FILENAME;
	case PT_NUMERIC_LIST : 
		BEGIN(numeric_list_state);
		yylval.str = yytext;
		return T_NUMERIC_LIST;
	case PT_STRING_LIST :
		BEGIN(string_list_state);
		yylval.str = yytext;
		return T_STRING_LIST;
	case PT_FILENAME_LIST :
		BEGIN(filename_list_state);
		yylval.str = yytext;
		return T_FILENAME_LIST;
	default :
		abort();
	}
				}
<yesno_state>{space}*[Nn][Oo]	|
<yesno_state>{space}*[Yy][Ee][Ss]	{
		yylval.str = yytext;
		BEGIN(0);
		return T_YES;
	}
<truefalse_state>{space}*[Tt][Rr][Uu][Ee]	|
<truefalse_state>{space}*[Ff][Aa][Ll][Ss][Ee] {
		yylval.str = yytext;
		BEGIN(0);
		return T_TRUE;
	}
<numeric_state>{space}*{digit}+	{
		yylval.str = yytext;
		BEGIN(0);
		return T_NUM;
	}
<string_state>{space}*[^\n]* {
		yylval.str = yytext;
		BEGIN(0);
		return T_STR;
	}
<filename_state>{space}*{filename} {
		yylval.str = yytext;
		BEGIN(0);
		return T_FILE;
	}
<numeric_list_state>{space}*{digit}+	{
		yylval.str = yytext;
		return T_LISTVAL;
	}
<string_list_state>[^\n:]*	{
		yylval.str = yytext;
		return T_LISTVAL;
	}
<filename_list_state>{space}*{filename}	{
		yylval.str = yytext;
		return T_LISTVAL;
	}
<numeric_list_state>\n	|
<string_list_state>\n	|
<filename_list_state>\n	{
		BEGIN(0);
		return T_ENDOFLIST;
	}
<*>\n	BEGIN(0);
<*>.	return yytext[0];
%%

