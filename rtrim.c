#include <sys/types.h>
#include <stdlib.h>

char * rtrim(char * str)
{
	size_t len = strlen(str);
	char * ptr = str + (len - 1);
	while (strchr(" \t\n\r", *ptr) != NULL)
	{
		*ptr = 0;
		ptr--;
	}

	return str;
}
