#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

void * memdup(const void * src, size_t size)
{
	void * retval = malloc(size);
	memcpy(retval, src, size);
	return retval;
}
