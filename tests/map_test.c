#include "test.h"
#include "../map.h"
#include <assert.h>
#include <string.h>

const struct {
	const char * key;
	const char * val;
} test1_vals[] = {
	{"iufiugkjs", "uhgliaalrigua"},
	{"iaulierb", "kjgfauglds"},
	{"i7qgrtievfdvgk7w", "ki37teuksdx"},
	{NULL, NULL},
};

void test1(void)
{
	int i;
	map_t * map = map_new(strcmp);

	for (i = 0; test1_vals[i].key != NULL; i++)
		map_insert(map, test1_vals[i].key, test1_vals[i].val);
	for (i = 0; test1_vals[i].key != NULL; i++)
		assert(strcmp((char*)map_get(map, test1_vals[i].key), test1_vals[i].val) == 0);

	map_free(map);
}

int main(void)
{
	test1();
	
	return 0;
}

