#include <iostream>
#include "LCFlexLexer.h"

extern "C" int yylex(YYSTYPE * lvalp, void * handlep)
{
	if (((libconf_t *)handlep)->yyin == NULL)
		((libconf_t*)handlep)->yyin = std::cin;
	if (((libconf_t *)handlep)->yyout == NULL)
		((libconf_t*)handlep)->yyout = std::cout;
	if (((libconf_t *)handlep)->Lexer == NULL)
		((libconf_t*)handlep)->Lexer = new LCFlexLexer((std::istream*)(((libconf_t*)handlep)->yyin), (std::ostream*)(((libconf_t*)handlep)->yyout), (libconf_t*)handlep);

	return ((LCFlexLexer*)(((libconf_t*)handlep)->Lexer))->yylex(lvalp);
}

