#include "../array.h"
#include "test.h"

int array_test1_sort_helper(const void * ptr1, const void * ptr2)
{
	return ptr1 - ptr2;
}

void array_test1(void)
{	/* test the sorting */
	int i, prev = 0, new;
	array_t * array = new_array(0);
	
	for (i = 0; i < 2 * ARRAY_DEFAULT_SIZE; i++)
		array_push_back(array, (void*)rnd(1,100));
	for (i = 0; i < 2 * ARRAY_DEFAULT_SIZE; i++)
		if (((int)array_get(array, i)) == 0)
			exit(1);
	array_sort(array, array_test1_sort_helper);
	for (i = 0; i < 2 * ARRAY_DEFAULT_SIZE; i++)
	{
		new = (int)array_get(array, i);
		if (new < prev)
			exit(1);
		prev = new;
	}

	free_array(array);
}

struct array_test2_t
{
	int index;
	char val[21];
};

int array_test2_compare(const void * ptr1, const void * ptr2)
{
	struct array_test2_t * t1 = (struct array_test2_t *)ptr1;
	struct array_test2_t * t2 = (struct array_test2_t *)ptr2;

	return t1->index - t2->index;
}

void array_test2(void)
{	/* test the searching */
	int i, j;
	array_t * array = new_array(0);
	struct array_test2_t * testval;
	struct array_test2_t * searchval;

	for (i = 0; i < 20; i++)
	{
		testval = (struct array_test2_t*)malloc(sizeof(struct array_test2_t));
		testval->index = rnd(1, 100);
		for (j = 0; j < 20; j++)
			testval->val[j] = rnd('a', 'z');
		testval->val[20] = 0;

		array_put(array, i, testval);
		if (i == 10)
			searchval = testval;
	}
	testval = array_search(array, searchval, array_test2_compare);
	if (strcmp(testval->val, searchval->val) != 0)
		exit(1);
	array_sort(array, array_test2_compare);
	testval = array_search(array, searchval, array_test2_compare);
	if (strcmp(testval->val, searchval->val) != 0)
		exit(1);

	free_array(array);
}

int main(void)
{
	array_test1();
	array_test2();

	return 0;
}

