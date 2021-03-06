%{
/* The parser for the JAIL language */
// includes go here
// defines go here
%}

%token FUNCTION_TK 		/* "function" */
%token END_BLOCK_TK 		/* '}' */
%token INT_LITERAL_TK 		/* [1-9][0-9]+ */
%token TYPE_TK 			/* "type" */
%token FP_LITERAL_TK 		/* ([0-9]\.|[1-9.]){1}[0-9]+ */
%token ACCESS_TK 		/* "access" */
%token ALIAS_TK 		/* "alias" */
%token BOOL_LITERAL_TK 		/* "true"|"false" */
%token CLASS_TK 		/* "class" */
%token SINGLETON_TK		/* "singleton" */
%token CHAR_LITERAL_TK 		/* . */
%token STRING_LITERAL_TK 	/* \"[^\"]*\" */
%token BEGIN_BLOCK_TK 		/* '{' */
%token SCOPE_TK 		/* "scope" */
%token EOS_TK 			/* ';' */
%token NAMESPACE_TK 		/* "namespace" */
%token CONSTRUCTOR_TK		/* "constructor" */
%token DESTRUCTOR_TK 		/* "destructor" */
%token VAR_TK			/* "var" */
%token UNIT_TK			/* "unit" */
%token INTERFACE_TK		/* "interface" */
%token IMPLEMENTATION_TK 	/* "implementation" */
%token ROOT_TK			/* "root" */
%token LEAF_TK			/* "leaf" */
%token ABSTRACT_TK		/* "abstract" */
%token IDENT_TK			/* [A-Za-z_]+[A-Za-z0-9_]* */
%token DOUBLE_COLON_TK		/* "::" */
%token PRIVATE_TK		/* "private" */
%token PROTECTED_TK		/* "protected" */
%token PUBLIC_TK		/* "public" */
%token USE_TK			/* "use" */
%token EXTENDS_TK		/* "extends" */
%%

/** 
 * General grammar 
 */
/* the goal of the parser is to parse one complete translation unit.. */
goal : translation_unit 
     ;

/* a translation unit is an optional unit declaration followed by any number
 * of use directives, followed by an interface section, followed by an
 * implementation, initialization and/or termination section in any order.  */
translation_unit : 
	unit_declaration use_directives interface_section
	| use_directives interface_section
	| translation_unit implementation_section
	;

/* A unit declaration is the unit keyword followed by an identifier 
 * Note that the unit declaration is special because it can't be part of a
 * declarations production */
unit_declaration :
	UNIT_TK IDENT_TK EOS_TK
	;

/* A sequence of use directive is any number of individual use directoves */
use_directives : /* empty */
	| use_directive use_directives
	;

/* A use directive is the use token followed by the name of the object to 
 * use, followed by a semicolon */
use_directive :
	USE_TK qualified_identifier EOS_TK
	;

/* An interface section is the interface keyword followed by a colon,
 * followed by any number of declarations */
interface_section :
	INTERFACE_TK ':' declarations
	;

/* An implementation section is the implementation keyword followed by a
 * colon, followed by any number of declarations and/or definitions */
implementation_section :
	IMPLEMENTATION_TK ':' implementation_section_freeform
	;

implementation_section_freeform : /* empty */
	| declarations implementation_section_freeform
	| definitions implementation_section_freeform
	;

/** 
 * Names and identifiers
 */
qualified_function_name :
	IDENT_TK
	| DOUBLE_COLON_TK qualified_function_name
	;

qualified_class_name :
	IDENT_TK
	| DOUBLE_COLON_TK qualified_class_name
	;

variable_name :
	IDENT_TK
	;

namespace_name :
	IDENT_TK
	;

scope_identifier : 
	PRIVATE_TK
	| PROTECTED_TK
	| PUBLIC_TK
	;
/** 
 * Literals
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
	| function_declaration
	;

/* A class declarationis always a forward declaration - just the class token
 * and the class name, followed by an end-of-statement token */
class_declaration :
	CLASS_TK class_name EOS_TK
	SINGLETON_TK class_name EOS_TK
	;

/* a namespace declaration is the namespace token, followed by its name, 
followed by a namespace body */
namespace_declaration :
	NAMESPACE_TK namespace_name namespace_body
	;

/* An alias declaration provides an alias for an already-declared class 
 * or function. For that, it needs a fully-qualified class/function
 * name */
alias_declaration :
	ALIAS_TK alias_name ':' CLASS_TK qualified_class_name EOS_TK
	| ALIAS_TK alias_name ':' FUNCTION_TK qualified_function_name EOS_TK
	;

/* a variable declaration is the variable's type followed by the variable's
 * name. There may be more than one variable declared at the same time, in
 * which case the variable names are separated by commas. */
variable_declaration :
	VAR_TK variable_name variable_attrs EOS_TK
	;

/* A function declaration is the function token followed by its name, the 
 * parameters it takes and its attributes. The attributes in the declaration
 * shall correspond exactly to the attributes in the corresponding definition
 * or the attributes shall be absent from the definition. If the function is
 * to have any attributes, they are mandatory in the declaration. */
function_declaration :
	FUNCTION_TK function_name function_params function_attrs EOS_TK
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
	inheritance_specifiers CLASS_TK class_name class_attrs class_body
	| inheritance_specifiers SINGLETON_TK class_name class_attrs class_body
	;

/* a set of inheritance specifiers is any combination of any the "root", "leaf"
 * and "abstract" keywords, in any order. Any of them can appear zero or one
 * times in the specifiers set, which can therefore be empty. */
inheritance_specifiers :
	/* empty */
	| ROOT_TK inheritance_specifiers
	| LEAF_TK inheritance_specifiers
	| ABSTRACT_TK inheritance_specifiers
	;

/* Class attributes define the from which parent classes the class is
 * derived and what scope those classes present.
 * The class attributes are defined by the extends token followed by 
 * an optional scope identifier (public, protected or private), followed 
 * by the names of the parent classes, or a colon followed by the
 * free-form version of the attributes (see below) */
class_attrs :
	/* empty */
	| EXTENDS_TK class_name_list
	| EXTENDS_TK scope_identifier class_name_list
	| ':' class_attrs_freeform
	;

/* A list of class names consists of at least one class name, or a 
 * comma-separated list of class names. The names may be fully 
 * qualified */
class_name_list :
	class_name
	| class_name ',' class_name_list
	;

/* The free-form format of class attibutes consists any number of
 * instances of either the extends keyword with a scope identifier
 * and a class name or the access token followed by an access 
 * control list. The instances are separated by commas. */
class_attrs_freeform : 
	EXTENDS_TK scope_identifier class_name
	| ACCESS_TK access_control_list
	| EXTENDS_TK scope_identifier class_name ',' class_attrs_freeform
	| ACCESS_TK access_control_list ',' class_attrs_freeform
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

/* Function attributes regroup the ACL, scope and (most importantly) the 
 * return type of the function. The order is undefined, but there are
 * two special forms: a colon followed by a type edentifier and an
 * optional scope identifier and a type token followed by a type
 * identifier.
 * If empty, the function does not return a value, has private scope and
 * is only executable */
function_attrs :
	/* empty */
	| ':' type_identifier scope_identifier 
	| ':' type_identifier
	| TYPE_TK type_identifier
	| ':' function_attrs_freeform
	;

function_attrs_freeform :
	/* empty */
	| TYPE_TK type_identifier function_attrs_freeform
	| SCOPE_TK scope_identifier function_attrs_freeform
	| ACCESS_TK access_control_list function_attrs_freeform
	;

/* A constructor is the 'constructor' keyword followed by its parameters
 * and its body. The parameters and body take the same form as those of
 * a normal function. */
constructor_definition : 
	CONSTRUCTOR_TK function_params function_body
	;

/* A destructor is the 'destructor' keyword followed by its body.
 * Destructors have no parameters. The body takes the same form as that
 * of a normal function */
destructor_definition :
	DESTRUCTOR_TK function_body
	;

%%

