#include "test.h"
#include "../queue.h"

void queue_test1(void)
{
	static char * strings[] = {
		"First",
		"Second",
		"Third",
		NULL,
	};
	queue_t * queue = new_queue();
	int i;
	
	for (i = 0; strings[i]; i++)
		queue_enq(queue, strings[i]);

	for (i = 0; strings[i]; i++)
		if (strcmp(queue_deq(queue), strings[i]))
			abort();
}

int main(void)
{
	queue_test1();
	return 0;
}

