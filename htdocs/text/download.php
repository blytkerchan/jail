<h1>Downloading released files</h1>
<p>Here, you will find the files most recently released by the project. Like 
most Open Source projects, the motto is "release early, release often". That
means that you may find alpha-stage software among the released files. This
page will tell you how to recognise them.</p>
<h2>File versioning</h2>
<p>There are five catagories of files that may be released by the project:
<ul>
<li><b>Snapshots</b><br/>
Snapshots provide a view on the project (or a particular branch of the project)
at a given time. Much like a photograph, no specific requirements are applied 
for a snapshot to be released.<br/>
Snapshots are basically released if a bug has been reported and is assumed to
have been fixed, but the developer that fixed the bug wants feedback from the
used base before making a "real" release (bumping version numbers, etc.). 
Snapshots are not guaranteed to have any kind of compatibility with any other
version, but are guaranteed <b>not</b> to be maintained - <b>do not</b> use
a snapshot for production software!<br/>
Snapshots can be recognised by their filename: they have the date the snapshot
was generated in their name - e.g. <code>package-<b>20040203</b>.tar.gz</code>.
<br/>
If more than one snapshot was generated on the same day, the second snapshot
will have a patch number: <code>package-20040203-<b>1</b>.tar.gz</code>.
</li>
<li><b>Alpha releases</b><br/>
Alpha releases are releases of code that has passed the testsuite at the time 
of release, but may not have a testsuite that is actually worth much and may
not have been tested on all of the target platforms.<br/>
Target platforms are (generally) all POSIX platforms for which we have the 
necessary architecture-dependant code (if any) but not all developers have all
platforms "handy" (especially Cygwin is sometimes hard to come by because it
runs on Windows, which is proprietary. Solaris, AIX and other UNIX platforms
suffer from the same problem, but also often run on different architectures, 
which means they may not always be target platforms).<br/>
Alpha releases are released to the general public when the developer thinks 
the source code has reached a stability that is good enough for production,
but has not empirical evidence to support that. Functional test cases may be
lacking to a certain extent and/or the code may never have been used in beta or
production-quality software.<br/>
Alpha releases should be used with caution and should be tested extensively to
make sure any lurking bugs are caught rapidly, which is what Alpha releases
are really for.<br/>
The filename of an Alpha release looks like this: <code>package-(version)-<b>alpha</b>(patchlevel)</code> (e.g. <code>libcontain-0.0-alpha1</code>)</li>
<li><b>Beta releases</b><br/>
Beta releases are much like Alpha releases, except that they are (much?) closer
to stability. As of a Beta release to the Release Candidate and Final releases,
no new features will be added to the released package. This is called a 
<i>Feature Freeze</i> (and generally takes form in creating a branch that will
become the RC and the Final release)<br/>
Beta releases should have a complete test suite (unlike Alpha releases for which
this is not a requirement) but may not have had any usage "in the wild". The
test suite should have passed on at least one target platform and tests on 
other platforms are encouraged (as they are with Alpha releases as well).<br/>
Using a Beta release in your software still warrants some caution (though not
as much as an Alpha release or a snapshot). Generally, you should not base 
software intended for production (final releases, stable releases, etc.) on
a Beta release of anything.<br/>
The filename of a Beta release looks like this: <code>package-(version)-<b>beta</b>(patchlevel)</code> (e.g. <code>libcontain-0.0-beta1</code>)</li>
<li><b>Release Candidate (RC) releases</b><br/>
RC releases are much like Beta releases, except that they are very, <u>very</u>
close to being Final releases. Once a package is released as an RC release, 
no more code cleanup is done, no more optimisation is done, etc. - only bugs
are fixed and testcases are added.<br/>
RC releases must pass the test suite - which must be complete (and is because
it must be complete at the Beta release stage) - on all target platforms for
the Final release.<br/>
The last of the RC releases must correspond <i>exactly</i> to the Final release
of the version in question.<br/>
RC releases can usually be used in production-quality software without too much
trouble (though you should be ready to supply a patch when the final release 
comes out, if the final release doesn't correspond to the RC release in 
question)<br/>
The filename of an RC release looks like this: <code>package-(version)-<b>rc</b>(patchlevel)</code> (e.g. <code>libcontain-0.0-rc1</code>).</li>
<li><b>Final releases</b><br/>
Final releases are (intended to be) stable. All bugs reported on Alpha, Beta 
and RC releases must have been treated. For all intents and purposes, a Final
release must be considered as final: there should be no more bugs to fix and
no more issues to resolve.<br/>
Ideally, between one Final release and the next, a Final release should never
have to be patched. That notwithstanding, bugfixes involving serious bugs 
(security issues, crashes, data corruptions, etc.) may have to be fixed, in
which case the patchlevel is increased.<br/>
The filename of a Final release looks like this: <code>package-version-patchelevel</code> (e.g. <code>libcontain-0.0-1</code>).
</li>
</ul>
</p>
<h2>Version numbers</h2>
<p>Unlike with some projects, the version numbers of the packages in the 
Jail-ust project don't reflect the stability of the code involved.</p>
<p>A version number is made up of at least three, but maybe more, parts:
<code>major.minor.[big-patch]-['alpha'|'beta'|'rc']patch</code><br/>
The parts, and their meanings, are stated in the table below
</p>
<table class="intext" width="100%">
<tbody>
<tr class="title"><td>name</td><td>bumped-if</td><td>description</td></tr>
<tr><td class="subject">major</td>
<td>Major, ground-breaking changes have occured</td>
<td>The major version number reflects the number of times major, 
ground-breaking changes have occured on the code. A major change could be 
re-factoring the design of the packaged component, a complete re-write of the 
source, etc.<br/>
Passing from 0 to 1 is not a reflection on the maturity of the code: we have 
the Alpha, Beta, RC and Final releases for that.</td></tr>
<tr><td class="subject">minor</td>
<td>Changes that require modifications of client code have occured, features 
have been added, etc.</td>
<td>The minor version number reflects non-trivial changes that will lead to a 
new Final release and that are not subject to the <i>Feature Freeze</i> or the
<i>Code Freeze</i> of the previous Final release.<br/>
Basically, this number reflects how many times new releases were made that 
contain new features, code cleanups, optimisations, etc. Normally, the 
compatibility with client code is retained as much as possible (but is, BTW, 
not directly reflected by the release version number).</td></tr>
<tr><td class="subject">big-patch</td>
<td>Non-trivial changes have occured on a Final release</td>
<td>The big-patch number reflects the number if non-trivial changes that have
occured on a Final release. Non-trivial changes are basically changes that 
change (some aspect of) the behaviour of the package on more than one target
platform. These include bugfixes etc. These do not include fixes of packaging
errors unless they have a real impact on the behaviour of the packaged 
component.</td></tr>
<tr><td class="subject">patch</td>
<td>Trivial changes have occured on a Final release (packaging etc.) or we're 
not in a Final release yet.</td>
<td>The patch"level" version number reflects how many times the same package
has been released either:
<ul>
<li>Before the Final release, at the same stage in the release process</li>
<li>In the Final release stage, without non-trivial changes to the code</li>
</ul>
</td></tr>
</tbody>
</table>
<p>To choose the release you want, pose yourself the following questions:
<ul>
<li><b>Do I need something thouroughly tested, for production-quality stable
software with as little risk as possible?</b><br/>
If so, you need a Final release or (at your option) a Release Candidate</li>
<li><b>Do I need something thouroughly tested, mature, but not necessarily
ratified by the developer(s) in charge?</b><br>
If so, you need a Release Candidate or (at your option) a Beta release</li>
<li><b>Do I want to participate in testing the component, reporting/fixing
bugs, thus accelerating the development and testing process so I can benefit
from stable, thouroughly tested software as soon as possible (while remaining
on the cutting edge)?</b><br/>
If so, you want a Beta release or (at your option) an Alpha release</li>
<li><b>Do I want to participate in testing the component, fixing bugs and
helping out in the development of nacent features?</b><br/>
If so, you need an Alpha release</li>
</ul>
Snapshots shouldn't be used unless you're invited to use them.
</p>
<p>Now that you've read all this, you can take a look at the packages offered
for download.</p>
