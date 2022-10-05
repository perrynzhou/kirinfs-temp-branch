/*************************************************************************
  > File Name: stl_cond.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Fri 05 Aug 2022 04:41:46 AM UTC
 ************************************************************************/

#include <stdio.h>
#include <assert.h>
#include "stl_cond.h"

stl_cond *stl_cond_alloc()
{
  stl_cond *c = (stl_cond *)calloc(1, sizeof(stl_cond));
  assert(c != NULL);
  if (stl_cond_init(c) != 0)
  {
    free(c);
    c = NULL;
  }
  return c;
}
int stl_cond_init(stl_cond *c)
{

  int rc = -1;
  pthread_mutexattr_t attr;
  if (c)
  {
    pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
    c->mtx = mut;
    rc = pthread_mutexattr_init(&attr);
    assert(rc != -1);
    rc = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
    assert(rc != -1);
    rc = pthread_mutex_init(&c->mtx, &attr);
    assert(rc != -1);
    rc = pthread_cond_init(&c->cond, NULL);
    assert(rc != -1);
    pthread_mutexattr_destroy(&attr);
    c->init = true;
  }
  return rc;
}

int stl_cond_deinit(stl_cond *c)
{
  int rc;

  if (!c->init)
  {
    return 0;
  }
  if (pthread_mutex_destroy(&c->mtx) != -1 && pthread_cond_destroy(&c->cond) != -1)
  {
    c->init = false;
    return 0;
  }
  return -1;
}

void stl_cond_signal(stl_cond *c, void *data)
{
  int rc;

  pthread_mutex_lock(&c->mtx);
  c->data = data;
  c->done = true;
  rc = pthread_cond_signal(&c->cond);
  assert(rc == 0);
  pthread_mutex_unlock(&c->mtx);
}

void *stl_cond_wait(stl_cond *c)
{
  int rc;
  void *data;

  pthread_mutex_lock(&c->mtx);
  while (c->done == false)
  {
    rc = pthread_cond_wait(&c->cond, &c->mtx);
    assert(rc == 0);
  }

  data = c->data;
  c->data = NULL;
  c->done = false;
  pthread_mutex_unlock(&c->mtx);

  return data;
}