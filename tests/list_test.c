#include "test.h"
#include "../list.h"

int compare_func(const void * val1, const void * val2)
{
	return strncmp((const char *)val1, (const char *)val2, 3);
}

int main(void)
{
	static char * strings[] = {
		"Hello, world!",
		"Bye, world!",
		NULL
	};
	static char * keys[] = {
		"Hel", "Bye", NULL
	};
	list_t * list = new_list(compare_func);
	int i;
	
	for (i = 0; strings[i]; i++)
		if (list_insert(list, strings[i]))
			abort();
	for (i = 0; strings[i]; i++)
		if (strcmp(strings[i], list_search(list, keys[i])))
			abort();

	return 0;
}

