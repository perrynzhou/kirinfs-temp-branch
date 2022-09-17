/*************************************************************************
  > File Name: stl_array.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Wed 23 Mar 2022 10:30:24 AM UTC
 ************************************************************************/
#ifndef __STL_ARRAY_H
#define __STL_ARRAY_H
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
typedef void (*stl_array_free_cb)(void *);
typedef void (*stl_array_traverse_cb)(void *);
typedef struct {
  size_t mem_size;
  size_t max_count;
  size_t cur_count;
  uint8_t *bitmap;
  void *data;
} stl_array;
stl_array *stl_array_alloc(size_t mem_size, size_t count);
int stl_array_init(stl_array *arr, size_t mem_size, size_t count);
int stl_array_insert(stl_array *arr, size_t index,void *data);
int stl_array_remove(stl_array *arr,size_t index);
int stl_array_swap(stl_array *arr, size_t index1, size_t index2);
int stl_array_replace(stl_array *arr, size_t index, void *data);
int stl_array_traverse(stl_array *arr, stl_array_traverse_cb traverse_cb);
size_t stl_array_len(stl_array *arr);
void *stl_array_data(stl_array *arr,size_t index);
void stl_array_deinit(stl_array *arr,stl_array_free_cb free_cb);
void stl_array_destroy(stl_array *arr,stl_array_free_cb free_cb);
#endif
