#include "test.h"
#include "../stack.h"

void stack_test(void)
{
	int i, max_i;
	stack_t * stack = new_stack();
	static char * strings[] = {
		"first",
		"second",
		"third", 
		NULL,
	};
	
	for (max_i = i = 0; strings[i]; i++, max_i++)
		stack_push(stack, strings[i]);
	for (i = max_i - 1; i >= 0; i--)
	{
		if (strcmp(stack_top(stack), strings[i]))
			abort();
		stack_pop(stack);
	}
}

int main(void)
{
	stack_test();
	
	return 0;
}

