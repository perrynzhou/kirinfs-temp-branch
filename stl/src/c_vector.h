/*************************************************************************
  > File Name: c_vector.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 21 Mar 2022 11:35:08 AM UTC
 ************************************************************************/

#ifndef _VECTOR_H
#define _VECTOR_H
#include "c_utils.h"
typedef struct
{
  size_t mem_size;
  void **ptr;
  size_t real_count;
  size_t cap_count;
  c_vector_free_func_cb free_cb;
} c_vector_t;

c_vector_t *c_vector_alloc(size_t mem_size, c_vector_free_func_cb free_cb);
int c_vector_init(c_vector_t *vec, size_t mem_size,c_vector_free_func_cb free_cb);
int c_vector_add(c_vector_t *vec, void *data);
void *c_vector_remove(c_vector_t *vec);

int c_vector_remove_range(c_vector_t *vec,size_t start_pos,size_t end_post);
int c_vector_add_at(c_vector_t *vec, size_t index, void *data);
int c_vector_remove_at(c_vector_t *vec, size_t index);

void *c_vector_get(c_vector_t *vec,size_t index);
int c_vector_traverse(c_vector_t *vec, c_vector_traverse_cb traverse_cb);
void c_vector_destroy(c_vector_t *vec);
void c_vector_deinit(c_vector_t *vec);

inline void *c_vector_first(c_vector_t *vec)
{
  if (vec != NULL && vec->real_count > 0)
  {
    return vec->ptr[0];
  }
  return NULL;
}
inline void *c_vector_last(c_vector_t *vec)
{
  if (vec != NULL && vec->real_count > 0)
  {
    return vec->ptr[vec->real_count - 1];
  }
  return NULL;
}
#endif
