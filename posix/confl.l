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

