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
#include <sys/types.h>
#include <sys/stat.h>
#ifdef MT_SAFE
#include <fstream>
#else
#include <stdio.h>
extern FILE * yyin;
extern int yydebug;
#endif
#include <libreplace/fexist.h>
#include "readconf.h"

// forward declaration - definition found in generated parser confp.c
extern "C" int yyparse(libconf_t * handle);

static int libconf_readconf_worker(libconf_t * handle)
{
//	yydebug = 1;
	if (handle->filename == NULL)
		return 0;
	try
	{
		if (handle->yyin != NULL)
#if MT_SAFE
			delete (std::istream*)handle->yyin;
#else
			fclose((FILE*)handle->yyin);
#endif
		if (fexist(handle->filename) != 0)
			return -1;
#if MT_SAFE
		handle->yyin = new std::ifstream(handle->filename);
#else
		handle->yyin = fopen(handle->filename, "r");
		yyin = (FILE*)handle->yyin;
#endif
		yyparse(handle);
#if MT_SAFE
		delete (std::istream*)handle->yyin;
#else
		fclose((FILE*)handle->yyin);
		yyin = NULL;
#endif
		handle->yyin = NULL;
	}
	catch (...)
	{
		return -1;
	}

	return 0;
}

extern "C" int libconf_readconf_global(libconf_t * handle)
{
	handle->filename = handle->global_config_filename;

	return libconf_readconf_worker(handle);
}

extern "C" int libconf_readconf_local(libconf_t * handle)
{
	handle->filename = handle->local_config_filename;
	
	return libconf_readconf_worker(handle);
}
