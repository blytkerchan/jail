/* $Id: test5.cc,v 1.1 2003/10/07 16:42:39 blytkerchan Exp $ */
/* Tests for the Hash class
   Copyright (C) 2002
   Ronald Landheer <info@rlsystems.net>

This file is part of the Real Life Systems Hash Library (libhash).

libhash is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

libhash is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with libhash; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */
/* Hash tests */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Hash.h"
#include "StringHash.h"
#include "NocaseStringHash.h"
#include "IniHash.h"

#ifndef rnd
	#define rnd(x,y) (x) + (int)(((((y) - (x))) * (rand() / (RAND_MAX + 1.0))) + 0.5)
#endif // rnd

int main(void) {
	IniHash *hash = new IniHash();
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