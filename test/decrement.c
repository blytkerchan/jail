#include "../include/decrement.h"

int main(void)
{
	int i = 1;

	atomic_decrement(&i);
	if (i != 0) abort();

	return 0;
}

