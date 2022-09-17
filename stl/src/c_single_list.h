/*************************************************************************
    > File Name: c_single_list.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 28 Mar 2022 07:07:47 AM UTC
 ************************************************************************/

#ifndef _C_SINGLE_LIST_H
#define _C_SINGLE_LIST_H
#include "c_utils.h"
#include "c_list_utils.h"
typedef struct
{
  size_t cur_count;
  size_t mem_size;
  c_single_list_node_t *head;
} c_single_list_t;
c_single_list_t *c_single_list_alloc(size_t mem_size);
int c_single_list_init(c_single_list_t *list, size_t mem_size);

int c_single_list_add(c_single_list_t *list, void *data);
int c_single_list_add_at(c_single_list_t *list, size_t index, void *data);

void *c_single_list_get_first(c_single_list_t *list);
void *c_single_list_get_last(c_single_list_t *list);
void *c_single_list_get_at(c_single_list_t *list, size_t index);

void *c_single_list_remove(c_single_list_t *list);
int c_single_list_remove_at(c_single_list_t *list, size_t index);

int c_single_list_traverse(c_single_list_t *list, c_single_list_traverse_cb traverse_cb);

void c_single_list_deinit(size_t mem_size);
void c_single_list_destroy(c_single_list_t *list);
#endif
