%{
/* The parser for the JAIL language */
// includes go here
// defines go here
%}
%union 	{
    char * str;
    int i;
    double f;
	}

%token BEGIN_BLK END_BLK
%token FUNCTION RETURN
%token IF THEN WHILE DO ITER
%token IDENTIFIER
%token STR_LITERAL
%token INTEGER
%token FLOAT
%left MOD
%left '-' '+' '*' '/'
%right '=' EXP

%%
program : /* empty */
	| program expr
	| program function
/*	| program declaration
	| program definition	*/
	;

block	  /* not empty */
	: BEGIN_BLK expr END_BLK
	| BEGIN_BLK exprs END_BLK
	;

function_dec /* not empty */
	: FUNCTION IDENTIFIER '(' params ')'
	;

function /* not empty */
	: function_dec ';'
	| function_dec block
	;

exprs	  /* not empty */
	: expr ';' expr
	| exprs ';' expr
	| block expr
	;

expr	: /* empty */
	| IDENTIFIER '(' params ')'
	| INTEGER
	| FLOAT
	| STR_LITERAL
	| '(' expr ')'
	| expr '*' expr
	| expr '/' expr
	| expr MOD expr
	| expr EXP expr
	| expr '+' expr
	| expr '-' expr
	| block
	| RETURN '(' expr ')'
	| statement
	| IDENTIFIER
	;

params	: /* empty */
	| expr
	| params ',' expr
	;

statement /* not empty */
	: IF '(' expr ')' THEN expr
	| WHILE '(' expr ')' expr
	| WHILE '(' expr ')' DO expr
	| DO expr WHILE '(' expr ')'
	| DO exprs WHILE '(' expr ')'
	| ITER '(' expr ')' expr
	;

%%

