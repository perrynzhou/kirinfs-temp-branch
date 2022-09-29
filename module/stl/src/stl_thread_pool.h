/*************************************************************************
  > File Name: stl_thread_pool.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Thu 04 Aug 2022 01:02:05 PM CST
 ************************************************************************/

#ifndef _STL_THREAD_POOL_H
#define _STL_THREAD_POOL_H
#include <pthread.h>
#include <stdatomic.h>
// support multi thread
typedef struct
{
  int id;
  pthread_t pthread;
  void *pool;
  atomic_size_t done_task_num;
} stl_thread;

typedef struct stl_thread_pool
{
  stl_thread **threads;
  volatile int num_threads;
  atomic_size_t num_threads_alive;
  atomic_size_t num_threads_working;
  pthread_mutex_t thcount_lock;
  pthread_cond_t threads_all_idle;
  atomic_size_t his_task_num;
  void **task_queue;
} stl_thread_pool;
typedef void (*stl_thread_do_task_func)(void *);
stl_thread_pool *stl_thread_pool_alloc(int num_threads);
int stl_thread_pool_init(stl_thread_pool *tp, int num_threads);
int stl_thread_pool_add_task(stl_thread_pool *tp, stl_thread_do_task_func func, void *arg_p);
void stl_thread_pool_wait(stl_thread_pool *tp);

int stl_thread_pool_running_threads(stl_thread_pool *tp);
void stl_thread_pool_deinit(stl_thread_pool *tp);
void stl_thread_pool_destroy(stl_thread_pool *tp);
void stl_thread_pool_dump(stl_thread_pool *tp);

#endif