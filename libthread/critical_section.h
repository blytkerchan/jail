#ifndef _LIBTHREAD_CRITICAL_SECTION_H_
#define _LIBTHREAD_CRITICAL_SECTION_H_

#include "thread.h"

typedef struct 
{
	lt_thread_t * head;
	lt_thread_t * tail;
} critical_section_t;

critical_section_t * critical_section_new(void);
void critical_section_free(critical_section_t * critical_section);
void critical_section_enter(critical_section_t * critical_section);
void critical_section_leave(critical_section_t * critical_section);

#define CRITICAL_SECTION_USE_SEMS 1 /* set to 0 to use live locks */

#endif // _LIBTHREAD_CRITICAL_SECTION_H_

