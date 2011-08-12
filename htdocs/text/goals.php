<h1><a name="Goals"></a>Goals</h1>
<p>The goals of the Jail project are:</p>
<ul>
<li>To make a very portable interpreted language, that will run on
<ul>
<li>any Linux system (be it Red Hat, Mandrake, SuSE, Debian or
other)</li>
<li>any modern Windows system except Windows CE (using <a href=
"http://cygwin.com">Cygwin</a>)</li>
<li>any BSD system (be it FreeBSD, OpenBSD, NetBSD or other)</li>
<li>any other POSIX system</li>
</ul>
</li>
<li>To make a very easy interpreted language, that can be used by
practically anyone
<ul>
<li>by clearly documenting every part of the language</li>
<li>by transparently converting from any scalar type to
another</li>
<li>by handling all memory allocation internally</li>
<li>by handling pass-by-value and pass-by-reference in a "natural"
way</li>
</ul>
</li>
<li>To make a very "powerful" language
<ul>
<li>by being object-oriented all the way</li>
<li>by implementing a small but complete instruction set</li>
<li>by implementing integrated database access</li>
<li>by implementing Perl-compatible regular expressions</li>
<li>by implementing a simple way to make graphical user interfaces
on top of X<br></li>
</ul>
</li>
<li>To make a secure language
<ul>
<li>e.g. by making the interpreter configurable as to what access
it should allow to system resources<br>
e.g. it should be possible to disallow file system access if the
administrator doesn't want you to have it (e.g. for downloaded
scripts embedded in HTML pages)</li>
<li>by taking security considerations into account from the very
start of the language's design</li>
</ul>
</li>
</ul>
