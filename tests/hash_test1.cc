/* $Id: hash_test1.cc,v 1.2 2004/04/08 15:01:19 blytkerchan Exp $ */
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
/* Hash tests */
#include "../Hash.h"
#include "test.h"

int write_helper(void* tt) {
	return(strlen((char*)tt) + 1);
} // write_helper()

int main(void) {
	Hash *hash = new Hash();
	int i, I;
	key_type key[100], **keys;
	bool keys_found[100];
	char *buffer, *buff_arr[100];
	
	printf("populating Hash..");
	for (i = 0; i < 100; i++) {
		key[i].size = 4 * sizeof(char);
		key[i].value = malloc(key[i].size);
		sprintf((char*)key[i].value, "%.3d", i);
		buffer = (char*)malloc(11 * sizeof(char));
		memset(buffer, 0, 11 * sizeof(char));
		for (I = 0; I < 10; I++) {
			buffer[I] = (char)rnd('A', 'z');
		} // for
		unless(hash->put(&key[i], buffer)) {
			printf("error\n");
			return(1);
		} // unless
		buff_arr[i] = buffer;
	} // for
	printf("ok\n");
	printf("Checking key existance..");
	for (i = 0; i < 100; i++) {
		unless(hash->contains(&key[i])) {
			printf("error\n");
			return(1);
		} // unless
	} // for
	printf("ok\n");
	printf("Checking key list..");
	keys = (key_type**)hash->keys();
	for (i = 0; keys[i]; i++) {
		if (keys[i]->size != 4) {
			printf("error\n");
			return(1);
		} // if
		I = atoi((char*)keys[i]->value);
		keys_found[I] = true;
	} // for
	for (i = 0; i < 100; i++) {
		if (keys_found[i] == false) {
			printf("error\n");
			return(1);
		} // if
	} // for
	printf("ok\n");
	printf("Checking contents..");
	for (i = 0; i < 100; i++) {
		buffer = (char*)hash->get(&key[i]);
		if (!buffer) {
			printf("error\n");
			return(1);
		} // if
		if (strcmp(buffer, buff_arr[i]) != 0) {
			printf("error\n");
			return(1);
		} // if
	} // for
	printf("ok\n");
	printf("Writing hash to disk..");
#if defined(_WIN32) && !defined(__CYGWIN__)
	if (hash->write("c:\\tt.hash", write_helper))
#else
	if (hash->write("/tmp/tt.hash", write_helper))
#endif
		printf("ok\n");
	else {
		printf("error\n");
		return(1);
	} // if
	printf("Removing entries from hash..");
	for (i = 0; i < 100; i++) {
		unless (hash->remove(&key[i])) {
			printf("error\n");
			return(1);
		} // unless
		if (hash->contains(&key[i])) {
			printf("error\n");
			return(1);
		} // unless
		if ((buffer = (char*)hash->get(&key[i])) != NULL) {
			printf("error\n");
			return(1);
		} // if
	} // for
	printf("ok\n");
	printf("Reading hash from disk..");
#if defined(_WIN32) && !defined(__CYGWIN__)
	if (hash->read("c:\\tt.hash")) 
#else
	if (hash->read("/tmp/tt.hash")) 
#endif
		printf("ok\n");
	else {
		printf("error\n");
		return(1);
	} // if
	printf("Checking key existance..");
	for (i = 0; i < 100; i++) {
		unless(hash->contains(&key[i])) {
			printf("error\n");
			return(1);
		} // unless
	} // for
	printf("ok\n");
	printf("Checking key list..");
	keys = (key_type**)hash->keys();
	for (i = 0; keys[i]; i++) {
		if (keys[i]->size != 4) {
			printf("error\n");
			return(1);
		} // if
		I = atoi((char*)keys[i]->value);
		keys_found[I] = true;
	} // for
	for (i = 0; i < 100; i++) {
		if (keys_found[i] == false) {
			printf("error\n");
			return(1);
		} // if
	} // for
	printf("ok\n");
	printf("Checking contents..");
	for (i = 0; i < 100; i++) {
		buffer = (char*)hash->get(&key[i]);
		if (!buffer) {
			printf("error\n");
			return(1);
		} // if
		if (strcmp(buffer, buff_arr[i]) != 0) {
			printf("error\n");
			return(1);
		} // if
	} // for
	printf("ok\n");

	delete hash;
	return(0);
} // main()
