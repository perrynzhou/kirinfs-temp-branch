/*************************************************************************
    > File Name: c_utils.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Mon 21 Mar 2022 11:50:59 AM UTC
 ************************************************************************/

#ifndef _C_UTILS_H
#define _C_UTILS_H
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/uio.h>
#include <inttypes.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/param.h>


typedef struct c_node_s {
  void *data;
   struct c_node_s *next;
}c_node_t;

typedef struct {
  void *data;
  c_node_t *next;
  c_node_t *prev;
}c_double_list_node_t;

typedef struct {
  void *data;
  c_node_t *next;
}c_single_list_node_t;


typedef int (*c_vector_traverse_cb)(void *);
typedef int (*c_vector_free_func_cb)(void *);

typedef uint32_t (*c_hashmap_hash_cb)(void *, size_t);
typedef int (*c_hashmap_traverse_cb)(void *,void *);
typedef int (*c_hashmap_key_compare_cb)(void *,void *,size_t);

typedef void (*c_hashmap_free_cb)(void *);

typedef int (*c_array_traverse_cb)(void *);
typedef void (*c_array_free_cb)(void *);

typedef int (*stack_traverse_cb)(void *);
typedef void (*stack_free_cb)(void *);

typedef int (*c_single_list_traverse_cb)(void *);
typedef void (*c_single_list_free_cb)(void *);

typedef int (*c_double_list_traverse_cb)(void *);
typedef void (*c_double_list_free_cb)(void *);


typedef int (*c_queue_traverse_cb)(void *);
typedef void (*c_queue_free_cb)(void *);


typedef int (*c_binary_heap_traverse_cb)(void *);
typedef void (*c_binary_heap_free_cb)(void *);
typedef int (*c_binary_heap_compare_cb)(void *,void *);
inline static c_node_t *c_node_alloc(void *data)
{
  c_node_t *node =(c_node_t *) calloc(1,sizeof(c_node_t));
  assert(node !=NULL);
  node->data = data;
  return node;
}
inline static void c_node_destroy(c_node_t *node)
{
  if(node !=NULL)
  {
    free(node);
  }
}
#endif
