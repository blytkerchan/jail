#include "../hash.h"
#include "test.h"

int compare_func(const void * k1, const void * k2)
{
	return(strcmp((const char*)k1, (const char*)k2) == 0);
}

unsigned int hash_func(const void * key)
{
	return *((const int *)key);
}

// GLIB_HASH
void test_glib(void)
{
	static char * keys[] = {
		"this is a key",
		"yet another key",
		"this is another key",	// this one gets the same index as the first one
		NULL
	};
	hash_t * hash = new_hash(GLIB_HASH, hash_func, compare_func);
	int i;
	char * key;
	
	for (i = 0; key = keys[i]; i++)
	{
		hash_put(hash, key, (void*)i);
	}

	for (i = 0; key = keys[i]; i++)
	{
		if (hash_get(hash, key) != (void*)i)
		{
			abort();
		}
	}
}

int main(void)
{
	test_glib();
	return(0);
}

