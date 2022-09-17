#ifndef STL_MUTEX_H
#define STL_MUTEX_H

#include <pthread.h>
typedef struct
{
	pthread_mutex_t mtx;
} stl_mutex;

int stl_mutex_init(stl_mutex *mtx);

int stl_mutex_destroy(stl_mutex *mtx);

void stl_mutex_lock(stl_mutex *mtx);

void stl_mutex_unlock(stl_mutex *mtx);

#endif
