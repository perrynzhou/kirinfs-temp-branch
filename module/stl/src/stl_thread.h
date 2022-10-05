/*************************************************************************
    > File Name: stl_thread.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Thu 29 Sep 2022 06:23:47 AM UTC
 ************************************************************************/

#ifndef _STL_THREAD_H
#define _STL_THREAD_H
#include <pthread.h>
typedef void *(*stl_thread_func) (void *);
typedef struct {
  pthread_t tid;
  void *ctx;
}stl_thread;
stl_thread *stl_thread_aloc(void *ctx);
int stl_thread_init(stl_thread *thd,void *ctx);
int stl_thread_run(stl_thread *thd,stl_thread_func func);
void stl_thread_deinit(stl_thread *thd);
void stl_thread_destroy(stl_thread *thd);
#endif
