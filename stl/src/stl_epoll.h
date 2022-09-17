/*************************************************************************
    > File Name: stl_epoll.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Wed 07 Sep 2022 05:06:07 AM UTC
 ************************************************************************/

#ifndef _STL_EPOLL_H
#define _STL_EPOLL_H
#include <pthread.h>
#include <stdbool.h>
typedef int (*stl_epoll_io_func)(void *ctx, int fd);
typedef struct
{
  int id;
  int sfd;
  int efd;
  bool stop;
  void *stl_epoll_ctx;
  pthread_t thd;
} stl_epoll_thread;
typedef struct
{
  // epoll serverfd
  int sfd;
  int efd;
  // thread for works
  int num_threads;
  int num_active_threads;
  size_t max_event_size;
  // stop flag
  bool stop;
  // worker threads
  stl_epoll_thread *threads;
  // client fd handle function
  stl_epoll_io_func io_func;
  // func context
  void *io_func_ctx;
} stl_epoll;

stl_epoll *stl_epoll_alloc(char *addr, int max_num_threads, size_t max_event_size, stl_epoll_io_func io_func);
int stl_epoll_init(stl_epoll *sep, char *addr, int max_num_threads, size_t max_event_size, stl_epoll_io_func io_func);
int stl_epoll_run(stl_epoll *sep);
void stl_epoll_destroy(stl_epoll *sep);

int stl_epoll_incr_thread(stl_epoll *sep);
 int stl_epoll_deincr_thread(stl_epoll *sep);
inline int stl_epoll_stop(stl_epoll *sep)
{
  if (sep != NULL)
  {
    for (size_t i = 0; i < sep->num_threads; i++)
    {
      sep->threads[i].stop = true;
    }
    return 0;
  }
  return -1;
}
inline int stl_epoll_thread_num(stl_epoll *sep)
{
  if (sep)
  {
    return sep->num_threads;
  }
  return -1;
}
inline int stl_epoll_set_context(stl_epoll *sep, void *io_func_ctx)
{
  if (sep)
  {
    sep->io_func_ctx = io_func_ctx;
    return 0;
  }
  return -1;
}
#endif
