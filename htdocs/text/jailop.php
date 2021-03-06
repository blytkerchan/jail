<h1>Jail operators and precedence</h1>
<p id="summary">The operators of the Jail language supports all the standard mathematical
operators according to their &quot;usual&quot; precedence and with their most
&quot;natural&quot; meaning.</p>
<h2>Mathematical operators</h2>
<p>The different operators of the Jail language are given in the table below.
They are listed in order of precedence, the &quot;strongest&quot; operator 
first. The strength of the operator is defined by primary school logic</p>
<table width="100%" class="intext">
<tbody>
<tr class="title"><td>Operator</td><td>Description</td></tr>
<tr><td class="subject">^</td>
<td>Power operator: raise the left operand to the power of the right operand 
and return the result.<br/>
This operator calls the <code>power</code> function on the left operand, with
the (numeric) value of the right operand as its argument.
</td></tr>
<tr><td class="subject">*</td>
<td>Multiplication operator: multiply the left operand by the value of the
right operand.<br/>
This operator calls the <code>multiply</code> function of the left operand 
with the (numeric) value of the right operand as its argument.
</td></tr>
<tr><td class="subject">/</td>
<td>Division operator: divide the left operand by the value of the right 
operand.<br/>
This operator calls the <code>divide</code> function of the left operand with 
the (numeric) value of the right operand.
</td></tr>
<tr><td class="subject">mod</td><td>Module operator: calculate the remainder of
an integer division<br/>
This operator calls the <code>module</code> function on the left operand with
the (integer) value of the right operand.
</td></tr>
<tr><td class="subject">+</td><td>Addition operator: add two values<br/>
This operator calls the <code>add</code> function on the left operand with the
(numeric) value of the right operand.
</td></tr>
<tr><td class="subject">-</td><td>Substraction operator: substract two values<br/>
This operator calls the <code>substract</code> function on the left operand with
the (numeric) value of the right operand.
</td></tr>
</tbody>
</table>
<p>In each of these cases, the function to call is determined at compile-time. 
The compiler will first search for the proper function on the left operand 
matching the right operands type <i>as closely as possible</i> (i.e. without
requiring a cast) and, if it fails, will cast the right operand to it's numeric
(or integer) value. It will then search for the proper function taking the
numeric (or integer) value as parameter. If that function doesn't exist either,
the left operand is also cast to its numeric (or integer) value and the 
operation is done on those two (casted) values. The result will have the type
of the left operand.</p>
<p>This way of looking up the proper function to call for each operator allows
the operators to be extremely flexible:
<ul>
<li>by allowing the user (programmer) to override each operator, changing the
operator's default behaviour and even allowing the creation of new "primitive"
types<br/>
This would allow a mathematician to create a matrix object which supports 
each of these operations and simply write <code>matrix1 = matrix2 + matrix3
</code> without unnecessary restrictions.</li>
<li>by ensuring that the operation is always possible<br/>
Note, though, that if both values are cast the compiler should produce a 
warning.</li>
</ul>
</p>
<p>Each of the operators described above is left-associative. That basicalally
means that if the operator is found twice in the same expression, the operation
on the left (the one found first reading the line from left to right) is done
first. Basically, this means that <code>a - b - c</code> equals (exactly)
<code>(a - b) - c</code> and not <code>a - (b - c)</code>. For most intents
and purposes, you won't care about this. An exception, however, is when you
override the operator in a way that depends on how the association is handled.
</p>
<h2>assignment operators</h2>
<p>The following operators are right-associative, meaning that if they're found
twice in the same expression, the operation on the right is evaluated before the
one on the left. Due to the nature of these operators, their precedence is
irrelevant (they're all some sort of assignment operator).</p>
<table width="100%" class="intext">
<tbody>
<tr class="title"><td>Operator</td><td>Description</td></tr>
<tr><td class="subject">=</td><td>basic assignment operator: assign the value of
the right operand to the left operand.<br/>
This operator calls the <code>assign</code> function on the left operand with 
the <i>closest possible</i> value of the right operand. The closest possible
value means the value of the type closest to: <ul>
<li>a type accepted by one of the <code>assign</code> functions of the left 
operand</li>
<li>the type of the left operand or one of its decendants</li>
</ul>
If the left operand has no assign functions, or none of the assign functions 
accept the right operand's type as parameter, the compiler will not call the
assign function but will attempt to replace the left operand's value with those
of the right operand. If the right operand's type is a descendant of the left 
operand's type, the left operand's value will simply be replaced and no cast
will be effected to either operand. If the right operand has no parental 
relationship with the left operand and there is no assign function that can
do the assignment, the assignment is not possible and a compile-time error
will occur.<br/>
Note that a variable for which no type is specified gets its type at the moment
of its initialisation (i.e. its first assignment) so the initialisation of an
untyped variable cannot fail.
</td></tr>
<tr><td class="subject">+=</td><td>Add and assign operator.<br/>
This operator practically calls <code>add</code> on a temporary copy of the left
operand and calls <code>assign</code> on the left operand with the result of the
addition. The compiler may choose to optimise this a bit, though (i.e. the two
functions are not guaranteed to be called for this operation).</td></tr>
<tr><td class="subject">...</td><td>for each of the left-associative operators
described above, there shall be a right-associative assignment operator that
roughly conforms to the description of += above. Fill in the blanks where 
needed.</td></tr>
</tbody>
</table>
<h2>unary operators</h2>
<p>Unary operators are operators with only one operand. Whereas most operators
have a left and a right operand (and operate on either one as a function of
the other - e.g. <code>a * b</code> operates on <code>a</code> by using the 
value of <code>b</code>) unary operators are like functions that don't take
any parameters.</p>
<p>Unlike most operators, the unary operators don't allow any whitespace 
between the operator and the operand. The reason for this is simple: some
unary operators can be either left- or right-associative and must as such 
be distinguishable from eachother (e.g. the ++ operator can either be right-
or left-associative, but <code>i++</code> has a subtly different meaning 
from <code>++i</code>).<br/>
Other operators have ary equivalents. E.g. whereas <code>a + -1</code> is 
perfectly legal, <code>a + - 1</code> is not.<br/>
The table below contains the unary operators, in descending order of precedence</p>
<table class="intext" width="100%">
<tbody>
<tr class="title"><td>Operator</td><td>Association</td><td>Description</td></tr>
<tr><td class="subject">-</td><td style="text-align: center;">right</td><td>Negation: Negate the right-hand
operand (calculate 0 - the right-hand operand and return the result).<br/>
This operator tries to call the <code>negate</code> function on the right-hand
operand and simply return the value. If that doesn't work (because the 
right-hand operand doesn't have a negate function) the right-hand operand will
be cast to its numeric value and the negation will be done on that value.
</td></tr>
<tr><td class="subject">++</td><td style="text-align: center;">left</td><td>Post-increment operator: 
increment the left-hand operand and return the value <i>before incrementation</i><br/>
Calls the <code>increment</code> function on the left-hand operand at some 
point in time. This "point in time" is not guaranteed to be predictable. (This
basically means that you shouldn't post-increment the same operand twice in the
same expression, as that will lead to undefined results).<br/>
This merits an example: the following code will assign the value 1 to j, and
increment i.<br/>
<code>
i = 1;<br/>
j = i++;<br/>
</code>
</td></tr>
<tr><td class="subject">++</td><td style="text-align: center;">right</td><td>Pre-increment operator:
increment the right-hand operand and return the incremented value.<br/>
Unlike the post-increment operator, the pre-increment operator calls the <code>
increment</code> function immediatly, and returns the increment value.
</td></tr>
<tr><td class="subject">--</td><td style="text-align: center;">left</td>
Post-decrement operator: decrement the left-hand operand and return the value
<i>before decrementation</i> (see the post-increment operator to understand 
what this entails).<br/>
This operator calls the <code>decrement</code> function ont the left-side
operand at some point in time.
<td></td></tr>
<tr><td class="subject">--</td><td style="text-align: center;">right</td><td>
Pre-decrement operator: decrement the right-hand operand and return the
<i>decremented</i> value
</td></tr>
<tr><td class="subject">!</td><td style="text-align: center;">right</td><td>
Logical negation operator: return a boolean value that is logically opposite
to the boolean value of the right-hand operand.<br/>
Basically, this casts the right-hand operand to a boolean value and returns
the opposite of the result of the cast.</td></tr>
<!--
<tr><td class="subject"></td><td style="text-align: center;"></td><td></td></tr>
-->
</tbody>
</table>
<h2>Cast operators</h2>
<p>Many operators described above implicitly cast their operands to something
they can work with - many function calls do the same thing. Hence, there must
be some implicit or explicit cast operator. To define the semantics of casting
and the syntax of its operators, we should first define casting in a clear and
concise manner.</p>
<h3>What is casting, anyway?</h3>
<p>When we talk about casting an object (operand, variable, value, etc.) from 
some type A to some other type B, that basically means that we want to obtain 
the value of the object <i>as if</i> it were an object of type B.</p>
<p>Whenever the compiler needs an object of type A to have a value of type B
(because an operator has been applied to it, or it is used as a parameter to
a function that takes a parameter of type B) it will try to cast the object
by applying a cast operator to it. Applying a cast operator involves finding
the appropriate type to cast to. This type is either type B or one of its
decendants. The compiler will emit an error if it doesn't find the appropriate
cast operator.</p>
<p>The cast operator itself is not more (nor less) than a function of class A,
or one of its parents. This makes it unnecessary to have a special operator 
for explicit casts (such as C and C++ have) because you can just call the
function.</p>
<p>The function called when casting is called <code>cast</code>. The compiler
knows which cast function to take by the type it returns. (This, by the way,
is a feature that is available for all functions: distinguishing between
functions by their return type. It is also a feature languages like C and
C++ don't have..)</p>
<h2>Comparison operators</h2>
<p>All comparison operators call the same function in Jail: the 
<code>compare</code> function. This function is always called on the left-hand
operand and all comparison operators are left-associative.<br/>
The <code>compare</code> function should compare the current object with the
object passed in as a parameter and should:<ul>
<li>return -1 if the current object is lesser than the parameter</li>
<li>return 0 if the current object is equal to the parameter</li>
<li>return 1 if the current object is greater than the parameter</li>
</ul>
The operators are given in the table below, in order of decreasing precedence.
</p>
<table class="intext" width="100%">
<tbody>
<tr class="title"><td>Operator</td><td>Description</td></tr>
<tr><td class="subject">==</td><td>Equals operator. Return true if both operands are equal.</td></tr>
<tr><td class="subject">!=</td><td>Unequals operator. Return true if both operands are not equal</td></tr>
<tr><td class="subject">&lt;=</td><td>Lesser-than-or-equal operator. Returns true if the left operand is lesser than or equal to the right operand</td></tr>
<tr><td class="subject">&lt;</td><td>Lesser-than operator. Returns true of the left operand is lesser than the right operand</td></tr>
<tr><td class="subject">&gt;=</td><td>Greater-than-or-equal operator. Return true if the left operand is greater than or equal to the right operand</td></tr>
<tr><td class="subject">&gt;</td><td>Greater-than operator. Return true if the left operand is greater than the right operand.</td></tr>
</tbody>
</table>
<h2>Accessing operators</h2>
<p>Unlike most languages, Jail doesn't need special operators to access array,
list, hash, set, and other container objects: you can simply call the object
as a function. The object's "body" will be executed and it will return the
value you're looking for.
</p>
<h2>Operators missing from Jail</h2>
<p>Many languages offer bitwise operators on integers (shift left, shift right,
bitwise negation, etc.). Jail doesn't, because it simply doesn't know what a
bit is.<br/>
Many languages offer pointer/address dereferencing operators (arrows, 
address-of, etc.). Jail doesn't because it doesn't know what a pointer is and
it doesn't allow you to access memory directly.<br/>
Some languages have a "conditional" operator 
(<code>expr ? expr-if-true : expr-if-false</code>).
Too much time has been spent explaining how these work to C and C++ programmers
(and Perl programmers, but they seem to not use this operator often and when 
they do, they usually do so in throw-away code), so Jail doesn't have this
operator.</p>
<p>
In C, C++ and Perl, the comma (,) is an operator in some contexts. In Jail, it
is never an operator. If you want to "throw away" the result of an expression,
you simply don't use it.<br/>
This basically means that where you would do this in C:
<pre>
<b>for</b> (i = 0, j = 0; j &lt;= i; j++, i++)
{
    /* do something */
}
</pre>
You can do this in Jail:
<pre>
<b>for</b> (<b>begin</b> i = 0; j = 0; <b>end</b>; j &lt;= i; <b>begin</b> i++; j++; <b>end</b>)
<b>begin</b>
    [ do something ] 
<b>end</b>;
</pre>

</p>
