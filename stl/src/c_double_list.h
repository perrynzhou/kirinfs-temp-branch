/*************************************************************************
    > File Name: c_double_list.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Mon 28 Mar 2022 07:16:52 AM UTC
 ************************************************************************/

#ifndef _C_DOUBLE_LIST_H
#define _C_DOUBLE_LIST_H
#include "c_utils.h"
typedef struct
{
  size_t cur_count;
  size_t mem_size;
  c_double_list_node_t *head;
} c_double_list_t;
c_double_list_t *c_double_list_alloc(size_t mem_size);
int c_double_list_init(c_double_list_t *list, size_t mem_size);

int c_double_list_add(c_double_list_t *list, void *data);
int c_double_list_add_at(c_double_list_t *list, size_t index, void *data);

void *c_double_list_get_first(c_double_list_t *list);
void *c_double_list_get_last(c_double_list_t *list);
void *c_double_list_get_at(c_double_list_t *list, size_t index);

void *c_double_list_remove(c_double_list_t *list);
int c_double_list_remove_at(c_double_list_t *list, size_t index);

int c_double_list_traverse(c_double_list_t *list, c_double_list_traverse_cb traverse_cb);

void c_double_list_deinit(size_t mem_size);
void c_double_list_destroy(c_double_list_t *list);
#endif
