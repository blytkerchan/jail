/* Jail: Just Another Interpreted Language
 * Copyright (c) 2004, Ronald Landheer-Cieslak
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
#ifndef _PREPROCESSOR_PREPROC_H
#define _PREPROCESSOR_PREPROC_H

/* data type definitions */
typedef struct _preproc_inputfile_t
{
	char * 				filename;
	unsigned int 		lineno;
	void * 				state;
} preproc_inputfile_t;

typedef struct _preproc_token_def_t
{
	preproc_inputfile_t * inputfile;
	char * name;
	char * val;
} preproc_token_def_t;

/* variables found in preproc.c */
extern const char * include_search_path[];

/* variables found in preprocl.l */
extern char * curr_file;
extern int yylineno;
extern FILE * yyin;
extern FILE * yyout;

/* variables found in preprocp.y */
extern FILE * dev_null;

/* functions in preproc.c */
preproc_inputfile_t * new_preproc_inputfile(void);
void free_preproc_inputfile(preproc_inputfile_t * preproc_inputfile);

preproc_token_def_t * new_preproc_token_def(void);
void free_preproc_token_def(preproc_token_def_t * preproc_token_def);

const char * resolve_abs_filename(const char * filename);
const char * resolve_rel_filename(const char * filename);

/* found in preprocl.l */
int preprocl_getc(char * buffer, size_t max_size);
int preprocl_puts(const char * s);
int preprocl_putf(const char * filename);
int yylex(void);

#endif // _PREPROCESSOR_PREPROC_H
