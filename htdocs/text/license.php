<h1>The Jail License</h1>
<p id="summary">This page described the way the components of Jail are licensed 
and why. It also describes what kind of patches can be accepted for the project
and how to apply the licence to your patches.</p>
<h2>Introduction</h2>
<p>I (rlc) love Free Software - free as in speech. I also like Open software a
lot, for many of the same reasons. Those reasons are mainly that I believe the
best way of looking at software is as the result of a creative process that 
was once started to solve a problem. As problems need solving every day and
many of the problems that need solving today are the same problems that needed
solving yesterday, there's a chance that today's problem was already solved
yesterday.</p>
<p>I do not hate proprietary software: I've worked for companies that make such
software and I've collaborated in creation of such software. There is no danger
in hiring me to make proprietary software for you, but I do believe that 
proprietary software is generally of lesser quality than free software and 
that in most cases, the company that makes the proprietary software would be
well-served to open up their code.</p>
<p>What I love about free software is that I can do whatever I want with it - 
as long as I allow anyone else to do whatever they want with it too. One of
the things I have always done with free software is learn from it - the 
mistakes and the great ideas alike: unlike some, when I download a new software
package I (almost) never download the binary form: I download the source and
watch it compile. I then start looking at the source and learning from it.</p>
<p>For example: I once needed a DNS resolution algorithm that worked - and I
needed it quickly. There were no licensing limitations so I could use a GPLed
source without any problem. I took a look at the wget sources and found a 
linear hashing algorithm implemented in C, used to store the previously found
IP addresses for the domains already resolved. I studied the source, downloaded
a bunch of articles on the subject and implemented the same algorithm in C++.
Since then, I've re-implemented the same algorithm but added locks to it to
make it thread-safe - again in C++. The new C++ version has practically nothing
to do with the previous one (no code was re-used but it was written with the
same hands) and the previous one doesn't have much to do with the one found in
wget (different language, different model, re-written from scratch - just the
same algorithm at its base). In fact, aside from the fact that I remember what
brought be to the linear hashing algorithm, I don't remember much of anything
of the code in wget itself :)</p>
<p>For Jail, the same thing should be true: Jail is set up mainly for academic
excersize - and because my new-board daughter (born Oct 20th 2003) should have
access to an easily accessible language more powerful than the Basic I started
on when I was little. Hence, it should be possible for programmers to learn
from what is done here - and learn what they're doing as they go.</p>
<p>Again, I've read some books on the subject of language design, speak four
natural languages and six artificial ones and simply like software development
but this is the first time I've started designing a programming language, so
I basically have as much to learn as the next guy.</p>
<h2>Legal and moral considerations</h2>
<p>The GNU General Public License is nice, but it has one side-effect that 
makes it nearly unusable for commercial applications: if you use any GPLed 
code, you have to publish your code under a GPL-or-compatible license.</p>
<p>Many projects have suffered from this side-effect. Other projects have 
circumvented it (Samba, for example, makes an exception for Apple Computer Inc.
in the definition of "derived"). Others still have used it for their benefit
(Red Hat, formerly Cygnus, for example uses it for the Cygwin project to be
able to commercially license Cygwin: "pay me or free your customers").</p>
<p>The Jail license comes with a special clause that does much of the same
thing as the Cygwin license does: "help us out or be free". First, let's
take a look at the license:
<div style="color: blue;">
This is free software. You may distribute it and/or modify it and
distribute modified forms provided that the following terms are met:
<ul>
<li>Redistributions of the source code must retain the above copyright
notice, this list of conditions and the following disclaimer;</li>
<li>Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the distribution;</li>
<li>None of the names of the authors of this software may be used to endorse
or promote this software, derived software or any distribution of this 
software or any distribution of which this software is part, without 
prior written permission from the authors involved;</li>
<li>Unless you have received a written statement from Ronald Landheer-Cieslak
that says otherwise, the terms of the GNU General Public License, as 
published by the Free Software Foundation, version 2 or (at your option)
any later version, also apply.</li>
</ul>
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
</div>
Most of this is pretty straight-forward: the only thing not in a 
run-of-the-mill BSD license that is in the Jail license is that I (rlc) have
a right that others don't - i.e. to remove the restrictions of the GPL on 
the licensee.</p>
<h3>Why should you (rlc) have rights I don't?</h3>
<p>First off: this license only applies to those files that have the license
in them - typically any file I've written myself. I won't insist on applying
this license to any contributed software (though I'll be ahppy to accept any
constributed software that uses this license)<br/>
Second, the reason why I don't want to simply apply the GPL is stated above:
it has some side-effects that, in some cases, are less than desirable.</p>
<h3>Under what conditions will the GPL be waived?</h3>
<p>I will write the necessary written statement if the requesting party has 
made a significant contribution to the project. "Significant" means that the
project has really gone forward due to the contribution; "Contribution" can
be either financial or otherwise (code, research, resources, etc.).</p>
<h3>How does the license apply to contributed code?</h3>
<p>That's the contributer's choice - see below for what can be cosidered 
compatible licenses, but in general, any software that is not "copylefted"
can be included in Jail without too much trouble.</p>
<h2>Contributing - Legal issues</h2>
<p>Any non-trivial contribution has legal issues that must be resolved before
the contribution can be accepted. The issues are mainly:
<ul>
<li>Who owns the code?<br/>
If the contributor is not the owner of the code, he is technically not the 
contributor either. In that case, depending on the applicable license on the
code, we may need a written statement from the owner as to the fact that the
code can be contributed and published under a compatible licence.
</li>
<li>What license(s) is/are applied/applicable to the code?<br/>
Not all licenses are compatible with the Jail license: specifically the 
copylefting licences are not compatible unless the copyright owner is willing
to sign a copyright assignment form giving copyright to me (rlc)<br/>
Compatible licenses are all those licenses that allow the code to be used in
closed software (optionally with a notice that open source software was used).
However, "obnoxious" licenses (that say you should mention the owner of the 
code in all advertising material, for example) will not be accepted.
</li>
</ul>
If these conditions are met, contributions will be "thoughtfully considered" 
(I can't promise they will be accepted as-is: I haven't seen the contribution
yet, don't know how much of a change it is to the current code-base, don't 
know if it is interesting for the project, etc.
</p>
