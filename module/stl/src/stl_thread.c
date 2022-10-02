/*************************************************************************
    > File Name: stl_thread.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Thu 29 Sep 2022 06:23:52 AM UTC
 ************************************************************************/

#include <stdio.h>
#include "stl_thread.h"

int stl_thread_init(stl_thread *thd, void *ctx)
{
  if (thd && ctx)
  {
    thd->ctx = ctx;
    return 0;
  }
  return -1;
}
stl_thread *stl_thread_aloc(void *ctx)
{
  stl_thread *thd = NULL;
  if (ctx)
  {
    thd = (stl_thread *)calloc(1, sizeof(stl_thread));
    assert(thd != NULL);
    thd->ctx = ctx;
  }
  return thd;
}
int stl_thread_run(stl_thread *thd, stl_thread_func func)
{
  if (thd && func)
  {
    return pthread_create(&thd->ctx, NULL, func, thd->ctx);
  }
  return -1;
}
void stl_thread_deinit(stl_thread *thd)
{
  if (thd)
  {
    pthread_join(thd->tid, NULL);
  }
}
void stl_thread_destroy(stl_thread *thd)
{
  if (thd)
  {
    pthread_join(thd->tid, NULL);
    free(thd);
    thd = NULL;
  }
}