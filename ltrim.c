#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

char * ltrim(char * str)
{
	char * ptr = str;

	while (strchr(" \t\n\r", *ptr) != NULL)
		ptr++;
	if (str != ptr)
	{
		memmove(str, ptr, strlen(ptr) + 1);
	}

	return str;
}
