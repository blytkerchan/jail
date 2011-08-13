#ifndef _rwlock_h
#define _rwlock_h

struct rwlock_type;
typedef struct rwlock_type rwlock_t;

rwlock_t * rwlock_new(void);
void rwlock_free(rwlock_t * handle);
void rwlock_wlock(rwlock_t * handle);
int rwlock_try_wlock(rwlock_t * handle);
void rwlock_wunlock(rwlock_t * handle);
void rwlock_rlock(rwlock_t * handle);
int rwlock_try_rlock(rwlock_t * handle);
void rwlock_runlock(rwlock_t * handle);
void rwlock_upgrade(rwlock_t * handle);
int rwlock_try_upgrade(rwlock_t * handle);
void rwlock_downgrade(rwlock_t * handle);

#endif
