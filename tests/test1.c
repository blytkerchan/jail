/* Jail: Just Another Interpreted Language
 * Copyright (c) 2003-2004, Ronald Landheer-Cieslak
 * All rights reserved
 * 
 * This is free software. You may distribute it and/or modify it and
 * distribute modified forms provided that the following terms are met:
 *
 * * Redistributions of the source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer;
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the distribution;
 * * None of the names of the authors of this software may be used to endorse
 *   or promote this software, derived software or any distribution of this 
 *   software or any distribution of which this software is part, without 
 *   prior written permission from the authors involved;
 * * Unless you have received a written statement from Ronald Landheer-Cieslak
 *   that says otherwise, the terms of the GNU General Public License, as 
 *   published by the Free Software Foundation, version 2 or (at your option)
 *   any later version, also apply.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <assert.h>
#include "../libconf.h"

void test1(void)
{
	static char * argv[] = 
	{
		"./test1",
		"-DHELLO=\"world\"",
		NULL
	};
	static libconf_opt_t options[] = {
		{ "define", 'D', TP_YES, PT_STRING_LIST, "define", TP_YES, PT_STRING_LIST, "define a macro", DOE_WARNING },
		{ NULL, 0, TP_NO, PT_NONE, NULL, TP_NO, PT_NONE, NULL, DOE_ERROR }
	};
	int argc = 2;
	int rc = 0;
	libconf_t * handle = NULL;
	libconf_opt_t ** t_options = libconf_optconst(options);
	
	handle = libconf_init(SRCDIR"/global_conf_test1", NULL, t_options, NULL, argc, argv);
	free(t_options);
	if (!rc) rc = libconf_phase1(handle);
	if (!rc) rc = libconf_phase2(handle);
	if (!rc) rc = libconf_phase3(handle);
	if (!rc) rc = libconf_phase4(handle);
	if (!rc) rc = libconf_phase5(handle);
	assert(!rc);
	libconf_fini(handle);
}

int main(void)
{
	test1();
}
