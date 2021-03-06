<h1>Current discussions on defining the language</h1>
<p>
This page will describe the different features of the language as defined on 
<a href="http://sourceforge.net/mailarchive/forum.php?forum_id=34463">the jail-ust-lang list</a>.<br/>
All of the topics on this page are open for discussion on the jail-ust-lang
list.
</p>
<ul>
<li><a href="#tag003">"Everything is an item" (1)</a></li>
<li><a href="#tag004">Grammar and syntax</a></li>
<li><a href="#tag005">Error and exception handling</a></li>
<li><a href="#tag001">"Everything is an item" (2)</a></li>
<li><a href="#tag002">Soft typing: new keywords, inheritance, etc.</a></li>
</ul>
<p>
Stuff not yet treated here:
<ul>
<li>Pointers &amp; pass-by-reference vs. pass-by-value</li>
<li>"Items are not confined to the context they are defined in."</li>
<li>"is everything an object?"</li>
<li>Attribute privacy</li>
<li>Standard classes</li>
<li>Standard functions</li>
<li>Static data</li>
<li>const</li>
<li>memory management</li>
<li>Object security flags</li>
<li>Preprocessor tokens, comments, etc.</li>
</ul>
</p>
<hr/>
<a name="tag003"/>
<h2>Everything is an item</h2>
<p>A scalar, an array, a file, a function, the program itself. To
quote my little brother: "difference the what's?!". Of course, you
might run into trouble assigning a value to a function - and the
interpreter should warn you about that - but that doesn't mean it's
wrong to do so!</p>
<p>Of course, there are things you can do with one item that you
can't with another: it doesn't make sense to multiply a file by
two, unless it makes sense to multiply its contents by two.. The
interpreter should warn the programmer about that, but that's
another question: if the programmer decides to multiply a file,
he'll get a multiplied file.. It's just a question of deciding on
the semantics of the operation and the operation can be done.</p>
<div style="text-align: right; font-size: smaller;"<a href="#top">back to top</a></div>
<a name="tag004"/>
<h2>Grammar and syntax</h2>
<p>The grammar of Jail is under development. Basically, what we
need is a simple, straight-forward grammar to be used with a syntax
that is, also, as simple as possible.</p>
<p>The programmer shouldn't have to care about the type behind an
item: all they have to deal with are items and expressions, a
program just being a complex form of an expression. Hence, the
grammar should define rules for the use of items and the contents
of expressions, for example:</p>
<ul>
<li>An expression is one of the following:
<ul>
<li>an item</li>
<li>an expression, followed by an operator, followed by an
expression</li>
<li>an expression between brackets</li>
</ul>
</li>
<li>A function call is an item followed by zero or more expressions
between brackets</li>
</ul>
The restrictions on the grammar are those imposed by the parser
generator we use, which is GNU Bison. The complete grammar is not
complete yet, but is under development. If you want to have the
current snapshot, you can always mail me at <i>ronald at landheer
dot com</i> but it you want to discuss it, please mail to
<i>jail-ust-lang-subscribe at lists dot sourceforge dot net</i>.
That will subscribe you to the language discussion mailing list
which is meant to define the semantics, syntax and grammar of the
language.
<p>We also need to define what names the programmer can give to his
items. I've reserved anything that starts with an underscore for
keywords. We don't need many keywords at the moment - some of the
keywords I can think of at the moment are:</p>
<ul>
<li>PROGRAM to denote the program currently running</li>
<li>ERROR to denote an error</li>
<li>OK to denote all is well</li>
<li>BEGIN, END, WHILE, DO<br></li>
</ul>
Other than the reserved words any sequence of one or more
alphanumeric characters, starting with a letter, should be OK for
use. However, anything starting with an underscore will be reserved
for future, or internal, use.<br>
<p>Some symbols may be pre-defined, such as console, stdin, stdout,
stderr, etc. The complete list of pre-defined symbols and what they
are for has yet to be defined. Again, if you want to join the
discussion, please mail to <i>jail-ust-lang-subscribe at lists dot
sourceforge dot net</i>.</p>
<div style="text-align: right; font-size: smaller;"<a href="#top">back to top</a></div>
<a name="tag005">
<h2>Error and exception handling</h2>
<p>Anybody who's tried teaching C++ to non-programmers will have
run into the problem of explaining what an exception is: you can
throw them and catch them like a ball, they can be of any type that
can be thrown, they usually denote an error but they can be used as
any part of a design, etc. etc. Due to exceptions, you need smart
pointers (what's a pointer again?) your code needs to be
exception-safe, etc. etc. The problems with exceptions are as
numerous as those with threads, and some of them are related,
too.</p>
<p>Some errors can be expected and can be handled gracefully. For
example, if the programmer writes a division, the divisor might be
zero, and division by zero is mathematically impossible. In stead
of just breaking the program, the interpreter can "gracefully"
handle this by setting the result to "not a number" and outputting
a warning message to the standard error device.</p>
<p>No error should be fatal: the interpreter should never bail out
screaming - that will only confuse the user and the programmer
alike. OTOH, the principle of JIJO (Junk In Junk Out) should be
upheld: if you give the interpreter a buggy program you should
expect the interpreter to give you plenty of warnings, and perhaps
do a lot of no-ops, but <i>it should never bail out</i>.</p>
<p>OTOH, we can't decide aforehand how each error/exception should
be handled. The technical architecture of the Jail Virtual Machine
therefore has to include a way to handle different types of
exceptions, ranging from division-by-zero to access violations. For
this, the JailVM will use user-provided callback functions, which
will be called in a similar way to signal handlers. These can
either be written in Jail or in some other language.</p>
<div style="text-align: right; font-size: smaller;"<a href="#top">back to top</a></div>
<a name="tag001"/>
<h2>"Everything is an item" (2)</h2>
<p>IMHO, the following text (taken from jail-ust-lang) is very clear on the
subject:</p>
<p><i>
The concept, once understood, is actually a simple one: if everything is an
object, the mere fact that it's an object means it has certain attributes in
common with everything else.<br/>
For example: an apple and an orange are both fruits. Being a fruit means they
are used by the plant they came from to reproduice, and means that we can
usually eat it. Hence, both oranges and applies are part of the group "fruit"
which "inherits" from the group "food" and the group "reproductive device"
like this:
<pre>
                             +------+
                             | item |
                             +------+
                                |
                                /
                                |
                        +-------+---------+
                        |                 |
             +----------+----------+   +--+---+
             | reproductive device |   | food |
             +---------------------+   +------+
                        |                 |
                        +---------+-------+
                                  |
                              +---+---+
                              | fruit |
                              +-------+
                                  |
                            +-----+----+
                            |          |
                       +----+---+  +---+---+
                       | orange |  | apple |
                       +--------+  +-------+
</pre>
This ASCII art basically means that reproductive devices and foodstuff are
items (they both "inherit" from the "class" item) and fruit "inherits" from
both. Where are the oranges and apples in this graph? They inherit from the
class "fruit" and are therefore both fruits, but with some different
properties.<br/>
That's the whole idea of object-oriented design (OOD): classes (i.e. kinds of
objects) have different properties (attributes and functions) and can inherit
from eachother (i.e. the class orange is a child of the class fruit, sharing
the properties that make it a fruit but having some properties that make it
an orange (color, taste, etc.)<br/>
(...)<br/>
Now, then. How is all of this interesting for Jail? Jail is an object-oriented
language due to the "Everything is an item" paradigm. This means all you will
every see of an item is the item interface. The question is: what should this
interface do by default?<br/>
What I've come up with so far is this: every item has three functions by
default (functions that are, of course, also items):
<ul>
<li>construct<br/>
creates the item, initializes everything it owns to their default values</li>
<li>destruct<br/>
destroys the item</li>
<li>body<br/>
does whatever needs to be done when the item is called as a function</li>
</ul>
By default, each of these will be empty, except for the constructor, which will fill in local variables with their default values, etc. When no function name
is specified and the compiler finds code, it will be put in the body.<br/>
Any of these functions can be overridden.<br/>
</i>
<div align="right">From a mail to jail-ust-lang on <a href="http://sourceforge.net/mailarchive/forum.php?forum_id=34463&max_rows=25&style=flat&viewmonth=200308&viewday=18">Tue Aug 19 15:10:02 2003</a></div>
</p>
<div style="text-align: right; font-size: smaller;"<a href="#top">back to top</a></div>
<a name="tag002"/>
<h2>"Soft typing: new keywords, inheritance, etc."</h2>
<p>
The following text - taken from YA mail - describes a set of four keywords that brings the notion of types to Jail
</p>
<p>
Following the previous discussion on this list, I have somewhat changed the
typing model for the Jail language by introducing three new keywords (and one
which was already implied by the previous model, but not specified as such).<br/>
<ul>
<li>root</li>
<li>cast</li>
<li>extends</li>
<li>[of] type</li>
</ul>
The "extends" keyword specifies the parent of a new class. When ommitted, the
Item class is used - hence all classes still inherit from the Item super-class.
<pre>
        class Animal;
        class Cat extends Animal;
</pre>
Multiple inheritance is also possible:
<pre>
        class Animal;
        class Pet extends Animal;
        class Feline extends Animal;
        class Cat extends Pet, Feline;
or:     class Cat extends Pet and Feline;
</pre>
Precedence of inheritance is left-to-right - i.e. if both have a same property,
the one belonging to the class mentioned first is inherited and will be used
of called implicitly (but within Cat, it will still be possible to use a
Feline property present in both Pet and Feline: Feline::property will do the
trick).<br/>
Now, as for automatic downcasting - i.e. "everything is an Item". The hierarchy
of objects implicitly allows this behaviour *unless* it is specified that it
should not, using the new "root" keyword:
<pre>
root class Plant;
</pre>
this makes it impossible for a Plant to be seen as anything but a plant - and
as such makes it incompatible with all types below it in the hierarchy.
In the case of double inheritance, there may be two roots. If the wanted type is found somewhere up from either root, but below the type itself, without
casting to another branch in the hierarchy, it is compatible. I.e.:
<pre>
        root class Plant;
        class Flowery extends Plant;
        root class Shrubbery extends Plant;
        class Phaseolus extends Flowery, Shrubbery;
</pre>
Phaseolus will be compatible with Plant, despite Shrubbery being a root class,
because it inherits that compatibility from Flowery.<br/>
I expect this situation to be rather rare.<br/>
As for functions that expect a certain item type, there is the new "[of] type"
keyword (i.e. "of" is optional):
<pre>
function func (param of type File);
</pre>
The function func may now be sure that the parameter "param" is of type "File"
when it is passed - a type incompatible with File cannot be used and will cause a compile-time error.<br/>
This means that param can be:
<ul>
<li>an object of class File</li>
<li>an object of a class that inherits from File and is not rooted between the
  File class and itself</li>
<li>an object of a class that can be cast to File</li>
</ul>
The latter implies another new keyword: "cast", which will denote a cast
operator:
<pre>
        class MyClass
        begin
                cast of type File
                begin
                        [...]
                end;
        end;
</pre>
<div align="right">From a mail to jail-ust-lang on <a href="http://sourceforge.net/mailarchive/forum.php?forum_id=34463&max_rows=25&style=flat&viewmonth=200309&viewday=1">Tue Sep  2 13:38:24 2003</a></div>
</p>
