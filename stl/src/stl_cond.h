/*************************************************************************
  > File Name: stl_cond.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 05 Aug 2022 04:41:41 AM UTC
 ************************************************************************/

#ifndef _STL_COND_H
#define _STL_COND_H
#include <pthread.h>
#include <stdbool.h>
typedef struct  {
	bool init;
	bool done;
	void *data;
	pthread_cond_t cond;
	pthread_mutex_t mtx;
}stl_cond;

stl_cond *stl_cond_alloc();
int stl_cond_init(stl_cond *c);
void stl_cond_signal(stl_cond *c, void *data);
void *stl_cond_wait(stl_cond *c);
int stl_cond_deinit(stl_cond *c);
void stl_cond_destroy(stl_cond *c);
#endif
