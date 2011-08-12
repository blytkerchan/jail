/* $Id: hash_test5.cc,v 1.2 2004/04/08 15:01:19 blytkerchan Exp $ */
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
#include "../StringHash.h"
#include "../NocaseStringHash.h"
#include "../IniHash.h"
#include "test.h"

int main(void) {
	IniHash *hash = new IniHash;
	int i, p, q;
	char *pri_key[10], *sec_key[100], *buff_arr[10][100], *buffer;
	key_list_type **keys;
	bool key_found[10][100];

	printf("Populating hash..");
	for (p = 0; p < 10; p++) {
		pri_key[p] = (char*)malloc(3 * sizeof(char));
		sprintf(pri_key[p], "%.2d", p);
		for (q = 0; q < 100; q++) {
			sec_key[q] = (char*)malloc(4 * sizeof(char));
			sprintf(sec_key[q], "%.3d", q);
			buff_arr[p][q] = (char*)malloc(11 * sizeof(char));
			memset(buff_arr[p][q], 0, 11 * sizeof(char));
			for (i = 0; i < 10; i++) {
				buff_arr[p][q][i] = rnd('A', 'z');
			} // for
			hash->put(pri_key[p], sec_key[q], buff_arr[p][q]);
		} // for
	} // for
	printf("ok\n");
	printf("Checking keys..");
	keys = (key_list_type**)hash->keys();
	for (p = 0; p < 10; p++) {
		for (q = 0; q < 100; q++) {
			key_found[atoi(keys[p]->primary_key)][atoi(keys[p]->secondary_keys[q])] = true;
		} // for
	} // for
	for (p = 0; p < 10; p++) {
		for (q = 0; q < 100; q++) {
			if (key_found[p][q] != true) {
				printf("error\n");
				return(1);
			} // if
		} // for
	} // for
	printf("ok\n");
	printf("Checking key existance..");
	for (p = 0; p < 10; p++) {
		for (q = 0; q < 100; q++) {
			if (hash->contains(pri_key[p], sec_key[q]) == false) {
				printf("error\n");
				return(1);
			} // if
		} // for
	} // for
	printf("ok\n");
	printf("Checking contents..");
	for (p = 0; p < 10; p++) {
		for (q = 0; q < 100; q++) {
			buffer = (char*)hash->get(pri_key[p], sec_key[q]);
			if (!buffer) {
				printf("error\n");
				return(1);
			} // if
			if (strcmp(buffer, buff_arr[p][q]) != 0) {
				printf("error\n");
				return(1);
			} // if
		} // for
	} // for
	printf("ok\n");
	printf("Writing hash to disk..");
#if defined(_WIN32) && !defined(__CYGWIN__)
	if (hash->write("c:\\tt.hash"))
#else
	if (hash->write("/tmp/tt.hash"))
#endif
		printf("ok\n");
	else {
		printf("error\n");
		return(1);
	} // if
	printf("Removing entries from the hash..");
	for (p = 0; p < 10; p++) {
		for (q = 0; q < 100; q++) {
			if (!hash->remove(pri_key[p], sec_key[q])) {
				printf("error\n");
				return(1);
			} // if
		} // for
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
	printf("Checking keys..");
	keys = (key_list_type**)hash->keys();
	for (p = 0; p < 10; p++) {
		for (q = 0; q < 100; q++) {
			key_found[atoi(keys[p]->primary_key)][atoi(keys[p]->secondary_keys[q])] = true;
		} // for
	} // for
	for (p = 0; p < 10; p++) {
		for (q = 0; q < 100; q++) {
			if (key_found[p][q] != true) {
				printf("error\n");
				return(1);
			} // if
		} // for
	} // for
	printf("ok\n");
	printf("Checking key existance..");
	for (p = 0; p < 10; p++) {
		for (q = 0; q < 100; q++) {
			if (hash->contains(pri_key[p], sec_key[q]) == false) {
				printf("error\n");
				return(1);
			} // if
		} // for
	} // for
	printf("ok\n");
	printf("Checking contents..");
	for (p = 0; p < 10; p++) {
		for (q = 0; q < 100; q++) {
			buffer = (char*)hash->get(pri_key[p], sec_key[q]);
			if (!buffer) {
				printf("error\n");
				return(1);
			} // if
			if (strcmp(buffer, buff_arr[p][q]) != 0) {
				printf("error\n");
				return(1);
			} // if
		} // for
	} // for
	printf("ok\n");

	return(0);
} // main()
