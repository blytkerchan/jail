<h1>Types in Jail</h1>
<h2>Introduction</h2>
<p>By far most of the discussion on the Jail Language discussion list has been
about types in Jail: the way types are handled is of primordial importance to
the design of the language, as the way data is abstracted determines what you
can do with that data.</p>
<h3>A brief history of types</h3>
<p>In the beginning, when I first started thinking of this language and what
it should be like, I though no typing at all would be the best way to get rid
of the most common types of errors in programs, which are type errors. After
a very short while, I came to the conclusion that it is simply not possible to
forego typing altogether, as it would pretty soon become impossible to do 
anything without abstracting the data to be handled. (I pretty much knew that 
already, but sometimes you don't know you know something until you contest that
knowledge.)</p>
<p>Hence came the idea of making everything an object of some sort - an 
<i>Item</i>. As in real life, everything would be an item of some sort which
you can do pretty much anything with. Apples, cups of coffee, loafs of bread, 
etc.: they all share a number of attributes - you can pick them up, they are
physical, etc. The same should go for items in a computer: anything is just
an abstraction of something else when it comes to a computer - the computer
never actually deals with reality, but always deals with some abstraction of
that reality. Keeping that in mind, there is no reason for a programming 
language to not do exactly the same thing.</p>
<p>When I started talking about this idea on the jail-ust-lang group, another
member of the group quoted Bjarne Stoustrup (the original designer of one of
the most popular object-oriented, imperical programming languages today, being
C++) saying it's not much use to be able to put a battleship in a matchbox.</p>
<p>Though it might be useful for world peace, it would only be useful if you
can actually see the difference between a match and a battleship (before you
light your cigare with a battleship and blow your nose off).</p>
<p>I then remembered St Paul and his gardening in his letter to the Romans 
(the one in which we, Christian gentiles, are simply twigs grafted onto 
the Jewish olive tree). As in information/computer science, we often talk
about trees and forests when there is a hierarchy to be constructed, the link
suddenly became obvious: if we have a tree of objects that all inherit from
Item (the Adam-object), that doesn't mean that all objects are compatible
with eachother: someone may well have cut off a branch and planted it as a
separate tree. In effect, any object may have become the root of a new tree
at some point!</p>
<p>Hence, four new keywords were needed and described in a mail on 
<a href="http://sourceforge.net/mailarchive/forum.php?forum_id=34463&max_rows=25&style=flat&viewmonth=200309&viewday=1">Tue, Sept 2</a>.
</p>
<h3>A revised vision of the Jail object model</h3>
<p>Everything is still an "Item" and directly or indirectly inherits from the
Item type, just like we are all human and all indirectly inherit from the first
of all humans - Adam. This has one immediate consequence, as stated on 
<a href="http://sourceforge.net/mailarchive/forum.php?forum_id=34463&max_rows=25&style=flat&viewmonth=200308&viewday=18">Tue, Aug. 19</a>: 
<i>"if everything is an object, the mere fact that it's an object means it has 
certain attributes in common with everything else"</i></p>
<p>These attributes are the attributes the object inherits from the Item object.
Most notable consequences of this statement are the facts that:<ul>
<li>every object is constructable<br/>
But the moment at which an object is constructed is undefined: it won't be
constructed until it is needed.</li>
<li>every object is destructable<br/>
But the moment at which an object is destroyed is undefined: it may never be
destroyed at all - even if it has gone out of scope. In fact, if the interpreter
thinks it is too costly to destroy the object (because it is too busy doing 
something else) there is no reason why it should destroy the object.</li>
<li>the program never actually works on the class (type) itself, but always on
an instance of the class<br/>
This basically means that if you change some attribute in one object, the same
attribute in another object is not changed</li>
<li>an object may be executable<br/>
In the Jail model, even functions are objects - as are statements. This 
basically means you can assign a block of code to a variable without executing
the block of code.<br/>
Whether the object actually is executable would depend only on the access 
rights you have on the object - which means there should be some way to define
those.</li>
<li>every object is an abstraction of data<br/>
This basically means that even functions are abstractions of some data. In the
case of a function, that data is the compiler-generated bytecode. This implies
that you can change the contents of a function <i>at run-time</i> which means
you can change its behaviour at run-time..</li>
<li>different types of access the different objects are restrictable<br/>
You can change the access restructions to objects at compile-time and at 
run-time. You can't make the access restrictions more permissive, though: 
if you have blocked write-access to an object, the compiler/interpreter 
will not allow you to write to the object <i>ever again</i> (and will be 
able to use that restriction to optimise your use of the object)
</li>
<li>there are no primitive, non-object types<br/>
This is actually not really true: behind the scenes, there are some primitive
types (including pointers) used by the platform on which the Jail program is
run - it would become almost impossible to translate the language to something
the machine can understand otherwise. It is true, however, that you can't
directly <i>use</i> the primitive types - at least not without using the 
"native" features (i.e. implementing a Jail function or class in some language
other than Jail)</li>
</ul>
</p>
<h2>Classes vs. namespaces</h2>
<p>There are some non-obvious differences between classes and namespaces that
may need explaining. (OK, I admit, I just kicked in an open door). The table
below gives the major differences between the two, in more or less random 
order.</p>
<table class="intext" width="100%">
<caption>Differences between classes and namespaces in Jail</caption>
<tbody>
<tr class="title"><td width="50%">Class</td><td width="50%">Namespace</td></tr>
<tr><td style="border-right: thin dashed grey">Constructable</td><td>Not constructable</td></tr>
<tr><td style="border-right: thin dashed grey">Copyable</td><td>Not copyable</td></tr>
<tr><td style="border-right: thin dashed grey">Not writable by default<br/>
Meaning that by default, you can't add a function to a class after the class
has been declared.</td><td>Writable by default<br/>
Meaning that by default, you can add a function to a namespace after the 
namespace has already been declared:<ul>
<li>Not every function defined in a namespace must be previously declared in
the namespace's declaration and</li>
<li>the namespace's declaration is &quot;extensible&quot;</li>
</ul></td></tr>
<tr><td style="border-right: thin dashed grey">Can be executable<br/>
Meaning that if the class has a <code>body</code> function, the class itself
is executable as a function (in which case <code>body</code> is called</td><td>Cannot be executable<br/>
Though namespaces are allowed to have functions called <code>body</code>, those
functions will not have the same special treatment as equivalent <code>body</code>
functions in classes - i.e. you can't call a namespace as a function.
</td></tr>
<!--
<tr><td style="border-right: thin dashed grey"></td><td></td></tr>
-->
</tbody>
</table>
<p>Obviously, this means that classes and namespaces are very different. That
is mainly because they are meant for very different purposes: classes define
variable types whereas namespaces group them.</p>
<h3>What's in a namespace?</h3>
<p>Shakespear, possible history's least-known programmer, at least knew to ask 
the right questions (though he may have phrased the question a bit differently):
"What's in a namespace? Would a variable by any other name not be as, well, 
variable?"</p>
<p>Namespaces serve a very important purpose in programming: they limit the
visibility of declarations within the namespace to the namespace itself and
the namespaces within it. This basically means that the default (root) 
namespace doesn't get clobbered with whatever you add to other namespaces. I.e.:
<pre>
<b>namespace</b> MyNamespace
<b>begin</b>
    <b>function</b> my_function()
    <b>begin</b>
        <i>[ do something nice ... ]</i>
    <b>end</b>;

    <b>function</b> one_function()
    <b>begin</b>
        my_function();
    <b>end</b>;
<b>end</b>;

<b>function</b> my_function()
<b>begin</b>
    <i>[ do something different, but nice as well.. ]</i>
<b>end</b>;

<b>function</b> another_function()
<b>begin</b>
    my_function();
<b>end</b>;
</pre>
In this example, the two functions <code>my_function</code> don't clash 
because they are not in the same namespace. The function <code>one_function</code>
calls the function <code>MyNamespace::my_function</code> whereas 
<code>another_function</code> calls the function <code>my_function</code> in
the root namespace.</p>
<h2>Pre-defined types</h2>
<p>There are some pre-defined types in Jail that are generally necessary to do
just about anything. Those types are:</p>
<table class="intext" width="100%">
<caption>Jail pre-defined types</caption>
<tbody>
<tr class="title"><td>Type</td><td>Description</td></tr>
<tr><td class="subject">Item</td><td>This is the "superclass": the class from
which all other classes inherit. The only use for this class as a type is for
variables for which the type is unknown at the time they're defined.<br/>
Every class that does not explicitly extend another class implicitly extends
the Item class. Any class that explicitly extends another class has no direct
link with the Item class unless it is explicitly extended as well.<br/>
There is no need to explicitly extend the Item class in any case: it doesn't
add anything to any class that any other class doesn't already have in any 
case, as all it does is define the default semantics of a number of functions
(operators, mostly).</td></tr>
<tr><td class="subject" colspan=2 align="center">Scalar types</td></tr>
<tr><td class="subject">Scalar</td><td>The Scalar class is a class that can 
contain any scalar value. Scalar values are numeric values or string values.
In Jail, references are not scalar values as they are in some other languages
(notably Perl). All the following scalar-type classes inherit from this class.
<br/>
Unlike in some other languages, Scalars cannot be considered Containers in 
Jail: a Container is an object that contains other objects. Scalars in Jail
are objects that contains primitives. They are the only objects that can do
so and all primitives are wrapped in a scalar class - there is no way you can
use a primitive without using the Scalar wrapper class as the compiler will
simply not support it (you can, of course, implement your own wrapper class
around a native function, which is how Scalar classes are made).</td></tr>
<tr><td class="subject">Number</td><td>Objects of this type can store any
numeric value.</td></tr>
<tr><td class="subject">Integer</td><td>Objects of this type can store any
integer value. This class inherits from the Number class and can be up-cast
to it without loss of data. Casting a Number to an Integer is not possible,
though, because you would loose the data behind the decimal dot.</td></tr>
<tr><td class="subject">Character</td><td>Object of this type store printable
and non-printable characters. Many of the characteristics of this class are the
same as those of Integer but casting doesn't quite work the same way: casting
a character to a number or an integer interpretes the character and gives you
its numeric value (not its ASCII value).<br/>
This class can store any character defined in Unicode. There is no guarantee 
on how the character is stored internally (i.e. how many bits it uses, etc.).
</td></tr>
<tr><td class="subject">String</td><td>This is a bit of a special scalar, as
it is also a container of sorts.<br/>
The String class is basically an array of characters, which has many of the 
same characteristics as a normal array, except for the fact that it can only
contain characters and that strings are terminated by the NUL character (a
special non-printable character that exists only to denote the end of strings).<br/>
The String class cannot be used to contain binary data (whereas an array of 
characters could be used for that purpose).
</td></tr>
<tr><td class="subject" colspan=2 align="center">Container types</td></tr>
<tr><td colspan=2><p id="summary">Containers are objects that
can contain other objects. As such, they are mainly used as you would use a
desk drawer: you simply put your objects in them and forget about them until 
you need them - at which time you can simply retrieve them and do whatever 
you need them for.<br/>
When using a container, you should think, in advance, about what that container
should do so you can pick the right container to start with.</p></td></tr>
<tr><td class="subject">Array</td><td>An array is a statically dimensioned 
container. This basically means you can only size it once, after which you will
no longer be able to change its size.<br/>
There are no restrictions on the objects stored in an array that are not met
by the Item object.</td></tr>
<tr><td class="subject">Vector</td><td>A Vector is a dynamically dimentioned
container, which is not sorted. There are no special restrictions or 
requirements for the objects stored in a Vector. It is much like a class 
except that the size is dynamic which means you will always be able to add
new objects to the vector.<br/>
Unlike vectors in some other languages, there are no guarantees on where the
objects are placed in memory (i.e.: there is no guarantee that the objects
placed in the vector occupy contiguous space in memory). This is mainly because
there is no way for a Jail application to access memory directly, so these
guarantees are not needed. Due to this fact, vectors in Jail can be optimised
more easily.<br/>
The order of elements in the vector is guaranteed to remain the same regardless
of any actions (other than a sort) done on the vector - i.e. searching in
a vector doesn't sort the vector (or at least not definitely).</td></tr>
<tr><td class="subject">List</td><td>A List is a dynamically dimensioned
container, sorted according to the value of the object. Hence, the objects in
the list must have a <code>compare</code> function.<br/>
Contrary to vectors and arrays, lists are sorted. This means that it is much 
easier to search inside a list. Hence, lists are normally used when there is 
searching to be done.</td></tr>
<tr><td class="subject">Queue</td><td>A Queue is a dynamically sized container
that allows adding at one end (the tail) and removing at the other end (the 
head). It is much like a Vector except that it can't be sorted and you can 
only read the head element.<br/>
Queues are normally used for objects that have to "wait in line" to be used 
(such as messages that need to be treated, etc.)</td></tr>
<tr><td class="subject">DEQueue</td><td>A DEQueue is a Double-Ended Queue.
It behaves exactly like the Queue class except that you can add and remove
to/from either end of the queue.</td></tr>
<tr><td class="subject">Stack</td><td>A stack is a dynamically sized container
that allows adding and removing items on only one end (the top)<br/>
A stack behaves a lot like a stack of towels: the last one you put on it is
the first one you will use.</td></tr>
<tr><td class="subject">Heap</td><td>A heap is much like a stack, in that you
can put things on it and retrieve things from the top. The difference between
a heap and a stack is that a heap is sorted in such a way that either the 
smallest or the largest element is always on top. Objects stored in a heap 
therefore need to have a <code>compare</code> function.</td></tr>
<tr><td class="subject" colspan=2 align="center">Associative Container types</td></tr>
<tr><td colspan=2><p id="summary">Associative containers are objects that map
keys to values. Both the keys and the values are objects of some sort. There 
are no special restrictions on the values, however, according to the type of
associative container used, the restrictions on the keys may vary.</p></td></tr>
<tr><td class="subject">Map</td><td>A Map maps a key to a value by comparing
the key to other keys already in the map. Hence, the keys need to have a 
<code>compare</code> function<br/>
Use maps when you need fast key-to-value mapping that doesn't take too much
space in memory, or when you don't have a hash function (and don't want to 
write one).
</td></tr>
<tr><td class="subject">Hash</td><td>A Hash maps a key to a value by using a
computed integer value corresponding to the key. For this, it calls the <code>
hash</code> function on the key.<br/>
It is actually quite difficult to write a good hash function: the function 
needs to correspond to a number of very specific requirements (listed below)
and ideally needs to be very fast. If a good hash function is available, 
however, a hash is a lot faster than a map.<br/>
The requirements of a hash function are:<ul>
<li>the result value must be an integer</li>
<li>the result value must not be random</li>
<li>the result value from two "close" input values must not be close</li>
</ul>
Often-used hash functions algorithsm are algorithms like MD4, MD5, SHA, CRC32,
Adler-32, etc.<br/>
Use a hash when you need fast key-to-value mapping, memory is not an issue and
you have a good hash function. If memory is an issue, you can still use a hash
but should be aware that it is (much) less memory-efficient than a map.
</td></tr>
</tbody>
</table>
