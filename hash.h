#ifndef _HASH_H
#define _HASH_H

#include "Hash.h"
#include "StringHash.h"
#include "IntHash.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	NORMAL_HASH,
	STRING_HASH,
	INT_HASH,
} libhash_hashtype;
Hash * new_hash(libhash_hashtype hash_type);
void delete_hash(Hash * hash);
void *hash_get(Hash * hash, const void *key);
int hash_put(Hash * hash, const void *key, const void *value);
int hash_remove(Hash * hash, const void *key);
void ** hash_keys(Hash * hash);

#ifdef __cplusplus
}
#endif

#endif // _HASH_H
