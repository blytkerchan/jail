<h1>Development</h1>
<p>Development on the Jail preprocessor has started on Oct 1st 2003, with the
creation of the jail-ust-devel discussion group and the upload of the coding
standards I (rlc) use for most of my projects (which is a set of amendmends on
the GNU coding standards)</p>
<p>The preprocessor's goal will be to (evidently) preprocess Jail code, to
make it readible by the compiler itself. The preprocessor will need several 
libraries and (other) components that will be used by the other programs in
the Jail Interpreter Suite (compiler, linker, VM) as well. The first goals
will entail making those libraries/components.</p>
<h2>The Development Plan</h2>
<p>The first thing that will be implemented is a library of data containers,
derived from a library (libhash) I have previously developed. The original
libhash is not thread-safe but should be made so. It should also be extended
to make it easier to use and it should implement other hashing and storage 
algorithms, preferrably non-blocking ones.</p>
<p>The container library will need minimal memory management to work around 
certain problems. That memory management can be the first step to a 
full-fledged memory manager for the virtual machine.</p>
<p>After the container library, a lexical scanner will be needed for the 
preprocessor. This scanner will be at the heart of the preprocessor as it will
be used to recognise the different preprocessor tokens.</p>
<p>Whether or not the preprocessor will need a full-fledged parser depends on
how complicated the task of the preprocessor will be, which partly depends on 
the outcome of discussions on the jail-ust-lang and jail-ust-devel groups.</p>
<p>Aside from the preprocessor, we will need a compiler and the two will 
probably need to communicate with eachother and keep a database of known 
classes and their definitions. Hence, we will need a shared database library. 
That same library can be the basis of a more advanced database library that
might, if needed, allow access to external databases.</p>
<p>And so the list goes on. The plan is to start with the parts every other
part depends on, and work on it to get a minimal working implementation so we
can get started on other parts. The container library is such a part, so 
development on it is active. The memory manager is another such part.</p>
<h2>Status</h2>
<p>The project currently periodically releases the libcontain library. At the
time of this writing, May 4, 2004, preparations are being made to integrate
libmemory into libcontain and release both libmemory and libcontain.</p>
<p>Releases of libcontain are governed mostly by its own development plan,
which currently looks somewhat like this:
<table>
<tbody>
<tr class="title"><td>Milestone</td><td>Mission</td></tr>
<tr><td>0.1-beta1</td><td><ul><li>repair ``make distcheck''</ul></td></tr>
<tr><td>0.2</td><td><ul><li>make hash thread-safe</li><li>add heap</li><li>Integrate libmemory</li></ul></td></tr>
<tr><td>0.3</td><td><ul><li>add hash implementation that uses lists as buckets</li><li>deprecate the use of NULL as helper functions in hash interface</li><li>deprecate the use of the C++ implementation of the hash with the C interface</li></ul></td></tr>
<tr><td>0.4</td><td><ul><li>in the HASH, HASS and HASI file formats, add the number of entries in the hash and grow to fit on read</li></ul></td></tr>
<tr><td>???</td><td><ul><li>make template interfaces for each container</li></ul></td></tr>
</tbody>
</table>
While working according to this plan, for version 0.2, two new containers were
added: a heap and a map. Both are based on a binomial tree (which is why both
were added at the same time). Libmemory is being integrated at the moment, 
which leaves making the Hash implementation thread-safe.</p>
<p>It is, at present, unclear whether the glib hash implementation will stay
in the library: there are some major differences in the design of the glib 
implementation vis-&agrave;-vis the design of libcontain - most notably with
respect to the ownership of was is stored in the hash. It may be that the
implementation is either deprecated or removed before version 0.3, and/or that
it is not made thread-safe when the C++ implementation is.</p>
<p>The C++ implementation has some other flaws - mainly because of the fact
that my C++ skills were rather rusty back then. There may be a version 0.2
before it is made completely thread-safe, in which case the mission of making
it thread-safe will be moved to milestone 0.3 (and the other milestones will be
bumped).</p>
<p>For the time being, however, the plan still stands. Version 0.2 is under
development, and version 0.1 has an alpha version "in the wild" waiting
for feedback. Please feel free to test-drive the library a bit - I'm rather
sure you'll like it. Also note that the licensing of this free software is
rather relaxed for contributors, so if you want to use it in commercial, closed
software, the fact that this is free software should not be a show-stopper for
you.</p>
