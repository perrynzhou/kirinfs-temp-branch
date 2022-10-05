/*************************************************************************
    > File Name: stl_radix_tree.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Tue 13 Sep 2022 03:30:20 AM UTC
 ************************************************************************/

#ifndef _STL_RADIX_TREE_H
#define _STL_RADIX_TREE_H
#include <stdint.h>
#include <stdio.h>

typedef struct stl_radix_node_s stl_radix_node_t;

typedef struct stl_radix_node {
  struct stl_radix_node *right;
  struct stl_radix_node *left;
  struct stl_radix_node *parent;
  uintptr_t value;
} stl_radix_node;

typedef struct {
  stl_radix_node *root;
  stl_radix_node *free;
  char *start;
  size_t size;
} stl_radix_tree;

stl_radix_tree *stl_radix_tree_alloc(intptr_t preallocate);

intptr_t stl_radix32tree_insert(stl_radix_tree *tree, uint32_t key,
                                uint32_t mask, uintptr_t value);
intptr_t stl_radix32tree_delete(stl_radix_tree *tree, uint32_t key,
                                uint32_t mask);
uintptr_t stl_radix32tree_find(stl_radix_tree *tree, uint32_t key);


void stl_radix_tree_alloc(stl_radix_tree *tree);

#endif
