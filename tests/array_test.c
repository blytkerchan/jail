#include "../array.h"
#include "test.h"

int array_test1_sort_helper(const void * ptr1, const void * ptr2)
{
	return ptr2 - ptr1;
}

void array_test1(void)
{
	int i;
	array_t * array = new_array(0);
	
	for (i = 0; i < 2 * ARRAY_DEFAULT_SIZE; i++)
		array_push_back(array, (void*)rnd(1,100));
	for (i = 0; i < 2 * ARRAY_DEFAULT_SIZE; i++)
		fprintf(stderr, "%.3d, ", (int)array_get(array, i));
	fprintf(stderr, "\n");
	array_sort(array, array_test1_sort_helper);
	for (i = 0; i < 2 * ARRAY_DEFAULT_SIZE; i++)
		fprintf(stderr, "%.3d, ", (int)array_get(array, i));
	fprintf(stderr, "\n");
}

int main(void)
{
	array_test1();

	return 0;
}

