/*************************************************************************
    > File Name: stl_rbtree.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Tue 13 Sep 2022 03:29:22 AM UTC
 ************************************************************************/

#ifndef _STL_RBTREE_H
#define _STL_RBTREE_H
#include <stdint.h>
// abstract from nginx

typedef int (*stl_rbtree_key_cmp)(void *,void *);
typedef struct stl_rbtree_node
{
  void *key;
  struct stl_rbtree_node *left;
  struct stl_rbtree_node *right;
  struct stl_rbtree_node *parent;
  unsigned char color;
  void *data;
} stl_rbtree_node;

typedef struct
{
  stl_rbtree_node *root;
  stl_rbtree_node *sentinel;
  stl_rbtree_key_cmp  cmp;

} stl_rbtree;

stl_rbtree *stl_rbtree_alloc(stl_rbtree_key_cmp  cmp);
int stl_rbtree_init(stl_rbtree *tree,stl_rbtree_key_cmp  cmp);
int stl_rbtree_insert(stl_rbtree *tree, stl_rbtree_node *node);
int stl_rbtree_delete(stl_rbtree *tree, stl_rbtree_node *node);
stl_rbtree_node *stl_btree_search(stl_rbtree *tree, void *key);
int stl_rbtree_deinit(stl_rbtree *tree);
void stl_rbtree_destroy(stl_rbtree *tree);
#endif
