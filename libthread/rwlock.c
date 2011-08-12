#include "rwlock.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>

struct rwlock_type
{
	pthread_mutex_t state_lock;
	pthread_cond_t writers;
	pthread_cond_t readers;
	pthread_cond_t upgraders;
	uint32_t waiting_writers;
	uint32_t waiting_readers;
	uint32_t waiting_upgraders;
	uint32_t active_readers;
	uint32_t active_writers;
};

rwlock_t * rwlock_new(void)
{
	rwlock_t * retval;
	int rv;

	retval = malloc(sizeof(rwlock_t));
	assert(retval != NULL);
	memset(retval, 0, sizeof(rwlock_t));
	rv = pthread_mutex_init(&(retval->state_lock), NULL);
	assert(rv == 0);
	rv = pthread_cond_init(&(retval->writers), NULL);
	assert(rv == 0);
	rv = pthread_cond_init(&(retval->readers), NULL);
	assert(rv == 0);
	rv = pthread_cond_init(&(retval->upgraders), NULL);
	assert(rv == 0);

	return retval;
}

void rwlock_free(rwlock_t * handle)
{
	pthread_cond_destroy(&(handle->upgraders));
	pthread_cond_destroy(&(handle->readers));
	pthread_cond_destroy(&(handle->writers));
	pthread_mutex_destroy(&(handle->state_lock));
	free(handle);
}

void rwlock_wlock(rwlock_t * handle)
{
	pthread_mutex_lock(&(handle->state_lock));
	while (handle->active_writers != 0 || 
	       handle->active_readers != 0 || 
	       handle->waiting_readers != 0 || 
	       handle->waiting_upgraders != 0)
	{
		handle->waiting_writers++;
		pthread_cond_wait(&(handle->writers), &(handle->state_lock));
		handle->waiting_writers--;
	}
	handle->active_writers++;
	assert(handle->active_writers == 1);
	pthread_mutex_unlock(&(handle->state_lock));
}

int rwlock_try_wlock(rwlock_t * handle)
{
	pthread_mutex_lock(&(handle->state_lock));
	if (handle->active_writers != 0 || 
	    handle->active_readers != 0 || 
	    handle->waiting_readers != 0 || 
	    handle->waiting_upgraders != 0)
	{
		pthread_mutex_unlock(&(handle->state_lock));
		return -1;
	}
	handle->active_writers++;
	assert(handle->active_writers == 1);
	pthread_mutex_unlock(&(handle->state_lock));

	return 0;
}

void rwlock_wunlock(rwlock_t * handle)
{
	pthread_mutex_lock(&(handle->state_lock));
	handle->active_writers--;
	if (handle->waiting_upgraders)
		pthread_cond_signal(&(handle->upgraders));
	else if (handle->waiting_readers)
		pthread_cond_broadcast(&(handle->readers));
	else if (handle->waiting_writers)
		pthread_cond_signal(&(handle->writers));
	pthread_mutex_unlock(&(handle->state_lock));
}

void rwlock_rlock(rwlock_t * handle)
{
	pthread_mutex_lock(&(handle->state_lock));
	while (handle->active_writers != 0 || handle->waiting_upgraders != 0)
	{
		handle->waiting_readers++;
		pthread_cond_wait(&(handle->readers), &(handle->state_lock));
		handle->waiting_readers--;
	}
	handle->active_readers++;
	pthread_mutex_unlock(&(handle->state_lock));
}

int rwlock_try_rlock(rwlock_t * handle)
{
	pthread_mutex_lock(&(handle->state_lock));
	if (handle->active_writers != 0 || handle->waiting_upgraders != 0)
	{
		pthread_mutex_unlock(&(handle->state_lock));
		return -1;
	}
	handle->active_readers++;
	pthread_mutex_unlock(&(handle->state_lock));
	return 0;
}

void rwlock_runlock(rwlock_t * handle)
{
	pthread_mutex_lock(&(handle->state_lock));
	handle->active_readers--;
	if (handle->active_readers == 0)
	{
		if (handle->waiting_upgraders)
			pthread_cond_signal(&(handle->upgraders));
		else if (handle->waiting_writers)
			pthread_cond_signal(&(handle->writers));
		else if (handle->waiting_readers)
			pthread_cond_broadcast(&(handle->readers));
	}
	pthread_mutex_unlock(&(handle->state_lock));
}

void rwlock_upgrade(rwlock_t * handle)
{
	pthread_mutex_lock(&(handle->state_lock));
	while (handle->active_writers != 0 || handle->active_readers != 1)
	{
		handle->waiting_upgraders++;
		pthread_cond_wait(&(handle->upgraders), &(handle->state_lock));
		handle->waiting_upgraders--;
	}
	handle->active_readers--;
	handle->active_writers++;
	pthread_mutex_unlock(&(handle->state_lock));
}

int rwlock_try_upgrade(rwlock_t * handle)
{
	pthread_mutex_lock(&(handle->state_lock));
	if (handle->active_writers != 0 || handle->active_readers != 0)
	{
		pthread_mutex_unlock(&(handle->state_lock));
		return -1;
	}
	handle->active_readers--;
	handle->active_writers++;
	pthread_mutex_unlock(&(handle->state_lock));
	return 0;
}

void rwlock_downgrade(rwlock_t * handle)
{
	pthread_mutex_lock(&(handle->state_lock));
	handle->active_writers--;
	handle->active_readers++;
	assert(handle->active_writers == 0);
	if (handle->waiting_readers)
		pthread_cond_broadcast(&(handle->readers));
	pthread_mutex_unlock(&(handle->state_lock));
}

