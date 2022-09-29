#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include "stl_mutex.h"
#include <assert.h>

int stl_mutex_init(stl_mutex *mtx)
{
	int rc, rv;
	pthread_mutexattr_t attr;
	pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

	mtx->mtx = mut;

	rc = pthread_mutexattr_init(&attr);
	if (rc != 0) {
		return -1;
	}

	rc = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
	assert(rc == 0);

	rc = pthread_mutex_init(&mtx->mtx, &attr);

	rv = pthread_mutexattr_destroy(&attr);
	assert(rv == 0);

	return rc != 0 ? -1 : 0;
}

int stl_mutex_destroy(stl_mutex *mtx)
{
	int rc;

	rc = pthread_mutex_destroy(&mtx->mtx);
	return rc != 0 ? -1 : 0;
}

void stl_mutex_lock(stl_mutex *mtx)
{
	int rc;
	rc = pthread_mutex_lock(&mtx->mtx);
	assert(rc == 0);
}

void stl_mutex_unlock(stl_mutex *mtx)
{
	int rc;
	rc = pthread_mutex_unlock(&mtx->mtx);
	assert(rc == 0);
}
