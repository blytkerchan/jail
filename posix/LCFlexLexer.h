#ifndef _LIBCONF_POSIX_LCFLEXLEXER
#define _LIBCONF_POSIX_LCFLEXLEXER

#define YY_DECL int LCFlexLexer::yylex(void)
#include <istream>
#include <ostream>
#include <FlexLexer.h>
#include "../libconf.h"
#include "confp.h"

class LCFlexLexer : public yyFlexLexer
{
private :
	libconf_t * handle;
	YYSTYPE * lvalp;
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
