/*************************************************************************
    > File Name: c_vector.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 21 Mar 2022 11:35:18 AM UTC
 ************************************************************************/

#include "inc/c_vector.h"

#define C_VECTOR_INCR (32)

c_vector_t *c_vector_alloc(size_t mem_size, c_vector_free_func_cb free_cb)
{
  c_vector_t *vec = (c_vector_t *)calloc(1, sizeof(*vec));
  assert(vec != NULL);
  c_vector_init(vec, mem_size, free_cb);
  return vec;
}
int c_vector_init(c_vector_t *vec, size_t mem_size, c_vector_free_func_cb free_cb)
{
  if (vec != NULL)
  {
    vec->ptr = (void **)calloc(C_VECTOR_INCR, sizeof(void **));
    assert(vec->ptr != NULL);
    vec->mem_size = mem_size;
    vec->free_cb = (free_cb == NULL) ? (NULL) : (free_cb);
    vec->cap_count = C_VECTOR_INCR;
    vec->real_count = 0;
    return 0;
  }
  return -1;
}
inline static int c_vecvtor_grow(c_vector_t *vec, size_t size)
{
  if (vec != NULL)
  {
    vec->ptr = (void **)realloc(vec->ptr, vec->cap_count + size);
    if (vec->ptr != NULL)
    {
      vec->cap_count += size;
      return 0;
    }
  }
  return -1;
}
inline static void *c_vector_data_alloc(size_t mem_size, void *data)
{
  void *ptr = calloc(1, mem_size);
  assert(ptr != NULL);
  memcpy(ptr, data, mem_size);
  return ptr;
}
int c_vector_add(c_vector_t *vec, void *data)
{
  if (vec != NULL)
  {
    if (vec->real_count >= vec->cap_count && c_vecvtor_grow(vec, C_VECTOR_INCR) != -1)
    {
      return 0;
    }
      void *ptr = c_vector_data_alloc(vec->mem_size, data);
      assert(ptr != NULL);
      vec->ptr[vec->real_count++] = ptr;
  }
  return -1;
}
void *c_vector_remove(c_vector_t *vec)
{
  if (vec != NULL && vec->real_count > 0)
  {
    return vec->ptr[vec->real_count--];
  }
  return NULL;
}
int c_vector_remove_range(c_vector_t *vec, size_t start_pos, size_t end_pos)
{
  if (vec == NULL || start_pos < 0 || start_pos > end_pos || end_pos > vec->real_count)
  {
    return -1;
  }

    for (size_t i = start_pos; i < end_pos; i++)
    {
      void *ptr = vec->ptr[i];
      if (vec->free_cb != NULL)
      {
        vec->free_cb(ptr);
      }
      vec->ptr[i] = NULL;
    }
    vec->real_count--;
    return 0;
}

void *c_vector_get(c_vector_t *vec,size_t index)
{
  if(index >=0 && index < vec->real_count) {
    return vec->ptr[index];
  }
  return NULL;
}
int c_vector_remove_at(c_vector_t *vec, size_t index)
{
  if(index < vec->real_count)
  {
    size_t shift = (vec->real_count - index) * sizeof(void *);
    if(vec->free_cb!=NULL)
    {
      vec->free_cb(vec->ptr[index]);
    }
    memmove(vec->ptr[index],vec->ptr[index+1],shift);
    vec->real_count-=1;
    return 0;
  } 
  return -1;
}
int c_vector_add_at(c_vector_t *vec, size_t index, void *data)
{
  if (vec != NULL && index >=0 && index < vec->real_count)
  {
    size_t left_count = vec->cap_count-vec->real_count;
    if (left_count< C_VECTOR_INCR)
    {
      size_t grow_size = left_count +C_VECTOR_INCR;
      if (c_vecvtor_grow(vec, grow_size) != 0)
      {
        return -1;
      }
    }
    void *ptr = c_vector_data_alloc(vec->mem_size, data);
    memcpy(ptr, data, vec->mem_size);
    size_t shift = (vec->real_count - index) * sizeof(void *);
    memmove(vec->ptr[index + 1], vec->ptr[index], shift); 
    vec->ptr[index] = ptr;
    return 0;
  }
  return -1;
}

int c_vector_traverse(c_vector_t *vec, c_vector_traverse_cb traverse_cb)
{
  int ret = -1;
  if (vec != NULL)
  {
    for (size_t i = 0; i < vec->real_count; i++)
    {
      void *ptr = vec->ptr[i];
      if (traverse_cb != NULL && (ret = traverse_cb(ptr)) != 0)
      {
        break;
      }
    }
  }
  return ret;
}
void c_vector_destroy(c_vector_t *vec)
{
  c_vector_deinit(vec);
  if (vec != NULL)
  {
    free(vec);
    vec = NULL;
  }
}
void c_vector_deinit(c_vector_t *vec)
{
 c_vector_traverse(vec, vec->free_cb);
  vec->real_count = 0;
  vec->cap_count = C_VECTOR_INCR;
  if (vec->ptr != NULL)
  {
    free(vec->ptr);
  }
}

#ifdef C_VECTOR_TEST
#include <stdio.h>
#include <stdlib.h>
int vec_print(void *data)
{
  fprintf(stdout, "%s\n", (char *)data);

  return 0;
}
int vec_print2(void *data)
{

  fprintf(stdout, "%d\n", *(int *)data);
  return 0;
}
int vec_free(void *data)
{
  if (data != NULL)
  {
    free(data);
    data = NULL;
  }
}
int main()
{
  int n = 3;
  c_vector_t vec1;
  c_vector_init(&vec1, 0, C_TYPE_STRING, vec_free);
  // c_vector_t *c_vector_alloc(size_t mem_size,c_data_type_t d_type, c_vector_free_func_cb free_cb);

  c_vector_t *vec2 = c_vector_alloc(sizeof(int), C_TYPE_NUM, NULL);
  for (int i = 0; i < n; i++)
  {
    char buf[256] = {'\0'};
    snprintf((char *)&buf, 256, "key-%d", i);
    c_vector_add(&vec1, &buf);
    c_vector_add(vec2, &i);
  }
  c_vector_traverse(&vec1, vec_print);
  c_vector_traverse(vec2, vec_print2);

  c_vector_deinit(&vec1);
  c_vector_destroy(vec2);
}
#endif