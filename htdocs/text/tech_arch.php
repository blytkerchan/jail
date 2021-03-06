<h1><a name="Technical_architecture"></a>Technical
architecture</h1>
<p id="summary">
The technical architecture of the Jail language (notions like inheritance,
casting, the scope of symbols, etc.) is under active discussion on the 
jail-ust-lang discussion list. The text below ("Items are not confined to the 
context they are defined in") has been submitted to that list.<br>
You can join the discussion by mailing to 
jail-ust-lang-subscribe at lists dot sourceforge dot net or by clicking 
<a href="mailto:jail-ust-lang-subscribe@lists.sourceforge.net">here</a>.
</p>
<p>The folowing texts should be seen as drafts pertaining the technical
architecture of the language and the different components of the interpreter.
They should <i>not</i> be seen as "canonical" texts - at least as long as
they haven't been discussed and "canonicalized" yet.</p>
<p>As far as the language itself is concerned, the jail-ust-lang list is the
only place where anything can be decided. (See the <?php link_to_page("General", "progress", "progress page"); ?> for details.</p>
<p>As far as the architecture of the interpreter and its components are 
concerned, nothing has been decided yet. There will be a list for discussion
on that topic when the jail-ust-lang list has made enough progress to warrant
such efforts.</p>
<h2>The toolset: the parts of the interpreter</h2>
<p>The interpreter consists of a number of programs, the interface to which is
the program `jail': the compiler-driver.<br>
The entire toolset will consist of:
<ul>
<li> the preprocessor
<ul>
<li> gets all the used definitions from the headers
<li> removes all comments
<li> stores filenames and line numbers for reference in its output files (the
only comments that can still exist after preprocessing)
<li> resolves all macros
</ul>
<li> the compiler
<ul>
<li> translates all code to VM instructions (still human-readable)
<li> mangles symbol names
<li> performs all optimization
</ul>
<li> the assembler
<ul>
<li> translates the VM instructions into byte code (not human-readable)
</ul>
<li> the linker
<ul>
<li> resolves all resolvable symbols
<li> creates a single, loadable image
</ul>
<li> the interpreter (VM)
<ul>
<li> loads the linker-created image
<li> dynamically links the image with libraries it uses
<li> checks the integrity of the bytecode
<li> checks and sets security flags as needed
<li> executes the byte code
</ul>
</ul>
The platform will be named "Justice" (for Jail Uses Silly Terms In Common 
Expressions)</p>
<p>The different components will read and write different files:
<ul>
<li> the preprocessor
<ul>
<li> produces .ji files from .jail files. .jail files are human-written Jail
source files
<li> produces .js files from .jS files. .jS files are human-written JailVM 
assembly source files
</ul>
<li> the compiler
<ul>
<li> produces .js files from .ji files. .ji files are preprocessor-written Jail
source files
</ul>
<li> the assembler
<ul>
<li> produces .jo files from .js files. .js files are either created by the
compiler or by the preprocessor and contain JailVM assembly source code.
</ul>
<li> the linker
<ul>
<li> produces .ja files from one or more .jo files. .jo files are created by the
assembler.
</ul>
<li> the interpreter (VM)
<ul>
<li> read .ja files and interprete them.
</ul>
</ul>
Hence, the files are:
<table class\"intext" width="100%">
<tbody>
<tr class="title"><td>file extention</td><td>description</td></tr>
<tr><td class="subject">.jail</td><td>Jail source file</td></tr>
<tr><td class="subject">.ji</td><td>preprocessed Jail source file</td></tr>
<tr><td class="subject">.jS</td><td>JailVM assembler source file that needs preprocessing</td></tr>
<tr><td class="subject">.js</td><td>JailVM assembler source file</td></tr>
<tr><td class="subject">.jo</td><td>Justice object file</td></tr>
<tr><td class="subject">.ja</td><td>Justice archive file</td></tr>
</tbody>
</table>
</p>
<p>There are still some ideas I'm playing WRT the formats of these files. In any
case, the .jail, .ji, .jS and .js files will be human-readable. The .jo and .ja
files will not be human-readable</p>
