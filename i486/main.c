#include <stdio.h>
#include "compare_and_exchange.h"

int main(void)
{
	int * i = (int*)malloc(sizeof(int));
	int * j = 0;
	int * k = i; 

	printf("%d\n", compare_and_exchange(&k, &j, i));
	printf("%p, %p, %p\n", i, j, k);
	k = 0;
	printf("%d\n", compare_and_exchange(&k, &j, i));
	printf("%p, %p, %p\n", i, j, k);

	return 0;
}
 
