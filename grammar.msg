Date: Wed, 23 Jul 2003 22:46:57 +0200 (CEST)
From: Ronald Landheer-Cieslak <ronald (at) landheer (dot) com>
To: jail-ust-lang (at) lists (dot) sourceforge (dot) net
Subject: Jail language grammar
Content-Type: TEXT/PLAIN; charset=US-ASCII

Hello all,

The "all" part of the greeting may be a bit premature, as there are 
currently only two subscribers to this list, but hey - I'm an optimist, 
and this list *did* only start about a week ago..

Let me introduce myself: I am Ronald Landheer-Cieslak. I am a software 
developer in France and am the one that started this project. For the 
moment, I am the only one to have developed anything on the Jail language 
- I've tried some different grammars and syntaxes, but nothing much. The 
reason I started this project is to make a language my unborn daughter 
(Sarah) can understand - she hasn't been born yet, but by the time we have 
a working compiler she will be, and she should be able to understand the 
language by then. That gives us a couple of years of leaway..

I've just committed the site files and (more importantly) the proposed 
grammar to CVS. The grammar is in the parse.y file in the compiler 
directory. I'll explain it a bit here:

The "tokens" or "terminals" defined in the grammar can be understood as 
"keywords" or "identifiers" - i.e. words like "while", "do", "iter" and 
function and variable names. The terminals (tokens) currently defined are:
  token name	actual text
* BEGIN_BLK	"begin"
* END_BLK	"end"
* FUNCTION	"function"
* RETURN	"return"
* IF		"if"
* THEN		"then"
* WHILE		"while"
* DO		"do"
* ITER		"iter"
* IDENTIFIER	any identifier - i.e. any word that is not reserved
* STR_LITERAL	any strin literal - i.e. any sequence of characters 
		between quotes. Whether these will only be double quotes 
		or whether single quotes will work as well has not been 
		defined (and is not important for the grammar as the 
		lexical scanner will take care of that)
* INTEGER	any integer value
* FLOAT		any floating-point value

the following terminals are left-associative operators:
* MOD		"mod"
* '+', '-' '*' '/' (the obvious)

the following are right-associative operators:
* '='
* EXP		"exp"

In the grammar description, by convention, terminals (i.e. tokens) are 
written in UPPERCASE while non-terminals (i.e. the stuff defined in the 
grammar description) are written in lowercase. The entire program resolves 
to a single non-terminal, in our case "program", which, in our case, can 
be empty.

In the grammar description, I have not defined any actions for any of the 
(non-)terminals found yet, nor have I made it clear what the types of 
non-terminals and terminals are. The latter doesn't really matter, as Jail 
will be a very soft-typed language - i.e. everything is an item. 
Internally, though, it may be useful to distinguish between integers and 
floating-point values, as integers are much easier to work with for the 
computer.. we'll cross that bridge when we come to it.

in the grammar file, please note that I have commented out the part of the 
"program" non-terminal that says it can be a declaration or a definition - 
I haven't defined those non-terminals yet.

Now, then. Before you ask "what's a non-terminal again?", please read "The 
Concepts of Bison" in the Bison info page - Bison is the parser generator 
we will be using to generate this part of the compiler, and the words 
"terminal" and "non-terminal" will come back often when we talk about the 
grammar.. The Tutorial section of the Bison info page is very interesting, 
and I recommend it to anyone who wants to help define any language - 
including Jail.

Here's the definition of the "program" non-terminal - as proposed:
program : /* empty */
        | program expr
        | program function
/*      | program declaration
        | program definition    */
        ;

i.e. "a program can be empty, can be an expression or can be a function, 
or any combination of expressions an functions". Expressions are defined 
in the expr non-terminal:
expr    : /* empty */			expressions can be empty,
        | IDENTIFIER '(' params ')'	can be an identifier followed by a 
					(list of) parameters between brackets
        | INTEGER			can be an integer
        | FLOAT				or a floating-point value
        | STR_LITERAL			or a string literal
        | '(' expr ')'			or an expression (any of these) 
					between brackets
        | expr '*' expr			an expression followed by an 
        | expr '/' expr			operator followed by another 
        | expr MOD expr			expression
        | expr EXP expr
        | expr '+' expr
        | expr '-' expr
        | block				a block-statement
        | RETURN '(' expr ')'		return, and another expression 
					between brackets (I should 
					probably specify that this only 
					works for ending a function or 
					quitting the program - i.e. this 
					returns to the caller
        | statement			a compound statement
        | IDENTIFIER			or just an identifier
        ;

With these two, we run into the notion of recursivity: an expression is 
made up of an expression and something else, or just something else. I.e. 
non-terminals can recurse into themselves. This may be a bit complicated 
at first, but you'll get used to it. Just remember that any time you see 
"expr" you see the *entire list* of the definition of "expr". Like this:

x = (1 + 1) + 1
     -   -		two expressions ...
     -----		... become one 
    -------   -		another two ...
-   -----------		... together on the same level as another one ...
--------------- 	... and finally a single expression!

I hope this is clear..

OK.. the actual file parse.y is available in CVS. You can either run
$ export CVSROOT=:pserver:anonymous@cvs.sourceforge.net:/cvsroot/jail-ust
$ cvs login
password anonymous
$ cvs co compiler

to find compiler/parse.y, or try via WebCVS. If I have time, I'll also 
make it available on the webpage tomorow.

Now, then... this is a *draft* of a *proposal*. That means that this is 
intended to be discussed about to get the bugs out. At a given moment, 
this will lead to a well-defined grammar for an as-yet non-existant 
language, the implementation of the parser and (part of) the lexical 
scanner, the compiler, the VM, etc. The reason this mailing list exists is 
to discuss this grammar, the syntax and the semantics of the language.

In other words: your opinion, please.

Please be aware that it may take about 24h for the pserver CVS to be 
updated when CVS commits have been made - that's because pserver CVS is 
actually a backup server. Hence, because I've only just uploaded the file, 
the file may not be available yet as I write these words.. If your 
connection times out or something like that, please try again later (this 
is exactly the reason why the file has not been put on the web page yet).

rlc



