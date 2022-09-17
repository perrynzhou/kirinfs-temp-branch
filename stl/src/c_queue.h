/*************************************************************************
    > File Name: c_queue.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Mon 28 Mar 2022 07:18:24 AM UTC
 ************************************************************************/

#ifndef _C_QUEUE_H
#define _C_QUEUE_H
#include "c_utils.h"
typedef struct {
  void **ptr;
  size_t mem_size;
  size_t cur_count;
  size_t cap_count;
  c_queue_free_cb free_cb;
}c_queue_t;

c_queue_t *c_queue_alloc(size_t mem_size,c_queue_free_cb free_cb);
int c_queue_init(c_queue_t *q,size_t mem_size,c_queue_free_cb free_cb);
int c_queue_push(c_queue_t *q,void *data);
void *c_queue_pop(c_queue_t *q);
int  c_queue_traverse(c_queue_t *q,c_queue_traverse_cb traverse_cb);
void c_queue_deinit(c_queue_t *q);
void c_queue_destroy(c_queue_t *q);
#endif
