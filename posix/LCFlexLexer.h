/* Jail: Just Another Interpreted Language
 * Copyright (c) 2003-2004, Ronald Landheer-Cieslak
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
#ifndef _LIBCONF_POSIX_LCFLEXLEXER
#define _LIBCONF_POSIX_LCFLEXLEXER

#define YY_DECL int LCFlexLexer::yylex(void)
#include <istream>
#include <ostream>
#include <FlexLexer.h>

#include "../libconf.h"
#include <confp.h>

class LCFlexLexer : public yyFlexLexer
{
private :
	libconf_t * handle;
	YYSTYPE yylval;

public:
	LCFlexLexer(void) : yyFlexLexer() {}
	LCFlexLexer(const LCFlexLexer & mLCFlexLexer) 
		: yyFlexLexer(mLCFlexLexer), 
		  handle(mLCFlexLexer.handle) {}
	LCFlexLexer(
		std::istream * arg_yyin, 
		std::ostream * arg_yyout, 
		libconf_t * n_handle) 
		: yyFlexLexer(arg_yyin, arg_yyout), 
		  handle(n_handle) {}

	int yylex(void);
	
	int yylex(YYSTYPE *lvalp)
	{
		int rc;
		
		memcpy(&yylval, lvalp, sizeof(YYSTYPE));
		rc = yylex();
		handle->yylineno = yylineno;
		memcpy(lvalp, &yylval, sizeof(YYSTYPE));

		return rc;
	}
	
	~LCFlexLexer(void) {};
};

extern "C"
{

int yylex(YYSTYPE * lvalp, void * handlep);

}
#endif // _LIBCONF_POSIX_LCFLEXLEXER
