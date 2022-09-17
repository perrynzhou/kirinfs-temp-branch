/*************************************************************************
    > File Name: stl_ring_buffer.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Fri 05 Aug 2022 07:13:16 AM UTC
 ************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include "stl_ring_buffer.h"
#define STL_DEFAULT_RING_BUF_CAPACITY (64)

stl_ring_buffer *stl_ring_buffer_alloc(size_t capacity, uint32_t d_size, bool is_expand)
{
  stl_ring_buffer *rb = calloc(1, sizeof(stl_ring_buffer));
  assert(rb != NULL);
  stl_ring_buffer_init(rb, capacity, d_size, is_expand);
  return rb;
}
int stl_ring_buffer_init(stl_ring_buffer *rb, size_t capacity, uint32_t d_size, bool is_expand)
{
  if (rb)
  {
    rb->capacity = (capacity < STL_DEFAULT_RING_BUF_CAPACITY) ? STL_DEFAULT_RING_BUF_CAPACITY : capacity;
    rb->capacity = capacity;
    rb->buf_ptr = calloc(capacity, sizeof(char) * d_size);
    assert(rb->buf_ptr != NULL);
    rb->head = rb->tail = 0;
    return 0;
  }
  return -1;
}

int stl_ring_buffer_queue(stl_ring_buffer *rb, void *data)
{
  if (rb != NULL && data != NULL)
  {

    atomic_fetch_add(&rb->tail, rb->d_size);
    memcpy(rb->buf_ptr + rb->tail, data, rb->d_size);
    return 0;
  }
  return -1;
}
void *stl_ring_buffer_dequeue(stl_ring_buffer *rb)
{
  void *ptr = NULL;
  if (rb)
  {
    ptr = rb->buf_ptr[rb->head];
    atomic_fetch_add(&rb->head, rb->d_size);
  }
  return ptr;
}

void stl_ring_buffer_destroy(stl_ring_buffer *rb)
{
  if (rb)
  {
    stl_ring_buffer_deinit(rb);
    free(rb);
    rb = NULL;
  }
}
void stl_ring_buffer_deinit(stl_ring_buffer *rb)
{
  if (rb)
  {
    free(rb->buf_ptr);
    rb->buf_ptr = NULL;
  }
}
extern inline bool stl_ring_buffer_is_full(stl_ring_buffer *rb);
extern inline bool stl_ring_buffer_is_empty(stl_ring_buffer *rb);