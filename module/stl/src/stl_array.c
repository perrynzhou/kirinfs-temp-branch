/*************************************************************************
    > File Name: stl_array.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Wed 23 Mar 2022 10:30:11 AM UTC
 ************************************************************************/

#include "stl_array.h"
#include <stdbool.h>
static bool stl_array_data_is_empty(stl_array *arr, size_t index)
{
  return false;
}
stl_array *stl_array_alloc(size_t mem_size, size_t count)
{
  if (mem_size < 0 || count < 0)
  {
    return NULL;
  }
  stl_array *arr = (stl_array *)calloc(1, sizeof(stl_array));
  assert(arr != NULL);
  if (stl_array_init(arr, mem_size, count) != 0)
  {
    free(arr);
    return NULL;
  }
  return arr;
}
int stl_array_init(stl_array *arr, size_t mem_size, size_t count)
{
  if (arr != NULL)
  {
    arr->cur_count = 0;
    arr->max_count = count;
    arr->mem_size = mem_size;
    arr->data = calloc(count, sizeof(char) * mem_size);
    assert(arr->data != NULL);
    return 0;
  }
  return -1;
}
void stl_array_deinit(stl_array *arr, stl_array_free_cb free_cb)
{

  stl_array_traverse(arr, free_cb);
  if (arr != NULL)
  {
    free(arr->data);
    arr = NULL;
  }
}
void stl_array_destroy(stl_array *arr,stl_array_free_cb free_cb))
{

  stl_array_deinit(arr);
  if (arr != NULL)
  {
    free(arr);
    arr = NULL;
  }
}

int stl_array_add(stl_array *arr, void *data)
{
  if (arr != NULL && arr->cur_count < arr->max_count)
  {
    arr->data[arr->cur_count++] = calloc(1, arr->mem_size);
    memcpy(arr->data[arr->cur_count - 1], data, arr->mem_size);
    return 0;
  }
  return -1;
}
int stl_array_add_at(stl_array *arr, void *data, size_t index)
{
  if (index >= 0 && index < arr->max_count && (arr->cur_count + 1) < arr->max_count)
  {

    size_t shift = (arr->cur_count - index) * sizeof(void *);
    void *ptr = calloc(1, arr->mem_size);
    memmove(&arr->data[index + 1], &arr->data[index], shift);
    memcpy(ptr, data, arr->mem_size);
    arr->data[index] = ptr;
    arr->cur_count++;
    return 0;
  }
  return -1;
}
int stl_array_remove(stl_array *arr)
{
  if (arr != NULL && arr->cur_count > 0)
  {
    void *data = arr->data[arr->cur_count - 1];
    if (arr->free_cb != NULL)
    {
      arr->free_cb(data);
    }
    arr->data[--arr->cur_count] = NULL;
    return 0;
  }
  return -1;
}
int stl_array_remove(stl_array *arr, size_t index)
{
  if (index >= 0 && index < arr->max_count)
  {
    if (index < arr->cur_count)
    {
      size_t shift = (arr->cur_count - index) * sizeof(void *);
      void *ptr = arr->data[index];
      memmove(&arr->data[index], &arr->data[index + 1], shift);
      if (arr->free_cb != NULL)
      {
        arr->free_cb(ptr);
        ptr = NULL;
      }
      arr->data[arr->cur_count--] = NULL;
    }
    return 0;
  }
  return -1;
}
int stl_array_swap(stl_array *arr, size_t index1, size_t index2)
{
  if (arr != NULL && index1 >= 0 && index2 >= 0 && index1 != index2 && index1 < arr->cur_count && index2 < arr->cur_count)
  {
    void *origin  =  calloc(1,arr->mem_size);
    memcpy(origin,&arr->data[index2],arr->mem_size);
    memcpy(&arr->data[index2],&arr->data[index1],arr->mem_size);
    memcpy(&arr->data[index1],origin,arr->mem_size);
    free(origin);
    return 0;
  }
  return -1;
}
int stl_array_replace(stl_array *arr, size_t index, void *data)
{
  if (arr != NULL && index >= 0 && index < arr->max_count)
  {
    void *ptr = &arr->data[index];
    memcpy(&arr->data[index], data, arr->mem_size);
    return 0;
  }
  return -1;
}
int stl_array_traverse(stl_array *arr, stl_array_traverse_cb traverse_cb)
{
  if (arr != NULL)
  {
    for (size_t i = 0; i < arr->cur_count; i++)
    {
      if (stl_array_data_is_empty(arr, i) && traverse_cb != NULL)
      {
        traverse_cb(&arr->data[i]);
      }
    }
    return 0;
  }
  return -1;
}

size_t stl_array_len(stl_array *arr)
{
  size_t len = -1;
  if (arr != NULL && arr->cur_count > 0)
  {
    return arr->cur_count;
  }
  return len;
}
void *stl_array_data(stl_array *arr, size_t index)
{
  void *data = NULL;
  if (arr != NULL && index < arr->max_count)
  {
    data = &arr->data[index];
  }
  return data;
}
