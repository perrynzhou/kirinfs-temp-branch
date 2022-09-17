/*************************************************************************
    > File Name: c_queue.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 28 Mar 2022 09:04:40 AM UTC
 ************************************************************************/

#include "inc/c_queue.h"
#define C_QUEUE_INCR (8)
c_queue_t *c_queue_alloc(size_t mem_size, c_queue_free_cb free_cb)
{
  c_queue_t *q = (c_queue_t *)calloc(1, sizeof(c_queue_t));
  assert(q != NULL);
  if (c_queue_init(q, mem_size, free_cb) != 0)
  {
    free(q);
    q = NULL;
  }
  return q;
}
int c_queue_init(c_queue_t *q, size_t mem_size, c_queue_free_cb free_cb)
{
  if (q != NULL && mem_size > 0)
  {
    q->cur_count = 0;
    q->mem_size = mem_size;
    q->ptr = (void **)calloc(C_QUEUE_INCR, sizeof(void *));
    q->cap_count = C_QUEUE_INCR;
    q->free_cb = free_cb;
    return 0;
  }
  return -1;
}
int c_queue_push(c_queue_t *q, void *data)
{
  if (q != NULL && data != NULL)
  {
    if (q->cur_count >= q->cap_count)
    {
      size_t expand_size =q->cap_count + 2 * C_QUEUE_INCR;
      q->ptr = (void **)realloc(q->ptr, expand_size*sizeof(void *));
      if (!q->ptr)
      {
        return -1;
      }
      q->cap_count = expand_size;
    }
    void *ptr = calloc(1, q->mem_size);
    if (ptr != NULL)
    {
      memcpy(ptr, data, q->mem_size);
      q->ptr[q->cur_count++] = ptr;
      return 0;
    }
  }
  return -1;
}
void *c_queue_pop(c_queue_t *q)
{
  void *data = NULL;
  if (q != NULL && q->cur_count > 0)
  {
    data = q->ptr[0];
    size_t shift = (q->cur_count - 1) * sizeof(void *);
    memmove(&q->ptr[0], &q->ptr[1], shift);
    q->cur_count -= 1;
  }
  return data;
}
int c_queue_traverse(c_queue_t *q, c_queue_traverse_cb traverse_cb)
{
  int ret = -1;
  if (q != NULL && traverse_cb != NULL)
  {
    for (size_t i = 0; i < q->cur_count; i++)
    {
      if ((ret = traverse_cb(q->ptr[i])) != 0)
      {
        break;
      }
    }
  }
  return ret;
}
void c_queue_deinit(c_queue_t *q)
{
  if (q != NULL)
  {
    bool is_free_cb = (q->free_cb == NULL) ? (false) : (true);
    for (size_t i = 0; i < q->cur_count; i++)
    {
      if (is_free_cb)
      {
        q->free_cb(q->ptr[i]);
      }
    }
  }
}
void c_queue_destroy(c_queue_t *q)
{
  c_queue_deinit(q);
  if (q != NULL)
  {
    free(q->ptr);
    free(q);
    q = NULL;
  }
}