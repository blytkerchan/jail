#ifndef _libcontain_deque_h_
#define _libcontain_deque_h_

struct deque_;
typedef struct deque_ deque_t;

deque_t * deque_new(size_t size);
void deque_free(deque_t * handle);
void deque_push_front(deque_t * handle, void * val);
void deque_push_back(deque_t * handle, void * val);
void * deque_pop_left(deque_t * handle);
void * deque_pop_right(deque_t * handle);

#endif

