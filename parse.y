%{
/* The parser for the JAIL language */
// includes go here
// defines go here
%}

%%

/** 
 * General grammar 
 */
/* the goal of the parser is to parse one complete translation unit.. */
goal : translation_unit 
     ;

/* a translation unit is a series of zero or more declarations and definitions,
 * in any order. A translation unit may be empty. */
translation_unit : /* empty */
	| declarations translation_unit
	| definitions translation_unit
	;

/** 
 & Literals
 */
/* literals can be numbers (int or floating point), booleans (true or false),
 * single characters in single quites (e.g. 'c') or strings (e.g. "hello"). 
 * Any of these are tokens and thus interpreted as such by the lexer. */
literal :
	INT_LITERAL_TK
	| FP_LITERAL_TK
	| BOOL_LITERAL_TK
	| CHAR_LITERAL_TK
	| STRING_LITERAL_TK
	;
/** 
 * Declarations 
 */
/* a series of declarations consists of one or more declarations */
declarations : 
	declaration
	| declaration declarations
	;

/* a valid declaration declares either a class, a namespace, an alias or a 
 * variable.
 * Jail doesn't have primitive types but does have type aliases, which allow
 * the programmer to use different names for the same class (even if the 
 * different names are in different namespaces). */
declaration :
	class_declaration
	| namespace_declaration
	| alias_declaration
	| variable_declaration
	;

/* A class declarationis always a forward declaration - just the class token
 * and the class name, followed by an end-of-statement token */
class_declaration :
	CLASS_TK class_name EOS_TK
	;

/* a namespace declaration is the namespace token, followed by its name, 
followed by a namespace body */
namespace_declaration :
	NAMESPACE_TK namespace_name namespace_body
	;

/* An alias declaration provides an alias for an already-declared class. For
 * that, it needs a fully-qualified class name */
alias_declaration :
	ALIAS_TK fully_qualified_class_name EOS_TK
	;

/* a variable declaration is the variable's type followed by the variable's
 * name. There may be more than one variable declared at the same time, in
 * which case the variable names are separated by commas. */
variable_declaration :
	class_name variable_names EOS_TK
	;
/** 
 * Definitions 
 */
/* a series of definitions consists of one or more definitions */
definitions :
	definition
	| definition definitions
	;

/* In Jail, the only things that can be _defined_ are classes and functions,
 * which are basically a special kind of classes. Namespaces cannot be defined
 * because they have no body. Aliases can't be defined because that would 
 * consistute a duplicate definition of the alias' contents. */
definition :
	class_definition
	| function_definition
	;

/* A class definition is the class' modifiers, the class token, the class' 
 * name, the super (parent) classes it extends, and the body of the class,
 * in that order. The class name may be fully qualified */
class_definition :
	class_scope_modifiers CLASS_TK class_name class_super class_body
	| class_scope_modifiers CLASS_TK fully_qualified_class_name class_super class_body
	;

/* A function definition is the function token, followed by the name of the
 * function (which must be fully qualified if the function definition is not
 * inside a class or namespace body) the function's parameter list, the
 * function's attributes and the function's body, in that order. */
function_definition :
	FUNCTION_TK function_name function_params function_attrs function_body
	;

/* A function body is a block of statements, definitions and declarations in 
 * any order, (the function body contents) followed by an optional exception
 * handling block, followed by an optional error handling block. The function
 * body contents may be empty. */
function_body :
	BEGIN_BLOCK_TK function_body_contents END_BLOCK_TK exception_handling_block_opt error_handling_block_opt
	;

function_body_contents :
	/* empty */
	| declaration function_body_contents
	| definition function_body_contents
	| statement function_body_contents
	;

%%

