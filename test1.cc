/* $Id: test1.cc,v 1.1 2003/10/07 16:42:39 blytkerchan Exp $ */
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

#ifndef rnd
	#define rnd(x,y) (x) + (int)(((((y) - (x))) * (rand() / (RAND_MAX + 1.0))) + 0.5)
#endif // rnd
#ifndef unless
	#define unless(x) if (!(x))
#endif // unless

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
