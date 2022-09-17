/*************************************************************************
    > File Name: c_binary_heap.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Mon 28 Mar 2022 11:16:06 AM UTC
 ************************************************************************/

#ifndef _C_BINARY_HEAP_H
#define _C_BINARY_HEAP_H
#include "c_utils.h"
typedef struct  {
	void **ptr;
  size_t  cur_count;
  size_t cap_count;
  size_t mem_size;
	c_binary_heap_compare_cb cmp_cb;
  c_binary_heap_free_cb  free_cb;
}c_binary_heap_t;
c_binary_heap_t *c_binary_heap_alloc(size_t mem_size,c_binary_heap_compare_cb cmp_cb,c_binary_heap_free_cb free_cb);

int c_binary_heap_init(c_binary_heap_t *heap,size_t mem_size,c_binary_heap_compare_cb cmp_cb,c_binary_heap_free_cb free_cb);
int c_binary_heap_push(c_binary_heap_t *heap,void *data);
void *c_binary_heap_pop(c_binary_heap_t *heap);
int  c_binary_heap_traverse(c_binary_heap_t *heap,c_binary_heap_traverse_cb traverse_cb);
void c_binary_heap_deinit(c_binary_heap_t *heap);
void c_binary_heap_destroy(c_binary_heap_t *heap);

#endif
