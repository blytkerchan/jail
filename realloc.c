#include <stdlib.h>

void * rpl_realloc(void * ptr, size_t size)
{
	if (ptr == NULL)
	{
		if (size == 0)
			return malloc(1);
		return malloc(size);
	}
	if (size == 0)
		return realloc(ptr, 1);
	return realloc(ptr, size);
}

