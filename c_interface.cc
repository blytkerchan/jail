#include "hash.h"

extern "C" Hash * new_hash(enum libhash_hashtype hash_type)
{
	switch (hash_type)
	{
		case NORMAL_HASH :
			return(new Hash);
		case STRING_HASH :
			return(new StringHash);
		case INT_HASH :
			return(new IntHash);
		default :
			return(NULL);
	}
}

extern "C" void delete_hash(Hash * hash)
{
	delete hash;
}

extern "C" void *hash_get(Hash * hash, const void *key)
{
	return(hash->get(key));
}

extern "C" int hash_put(Hash * hash, const void *key, const void *value)
{
	return(hash->put(key, value) ? 0 : 1);
}

extern "C" int hash_remove(Hash * hash, const void *key)
{
	return(hash->remove(key) ? 0 : 1);
}

extern "C" void ** hash_keys(Hash * hash)
{
	return(hash->keys());
}
