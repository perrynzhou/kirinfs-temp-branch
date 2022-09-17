/*************************************************************************
    > File Name: stl_rbtree.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Tue 13 Sep 2022 03:29:28 AM UTC
 ************************************************************************/

#include <stdio.h>
#include "stl_rbtree.h"

#define stl_rbt_red(node) ((node)->color = 1)
#define stl_rbt_black(node) ((node)->color = 0)
#define stl_rbt_is_red(node) ((node)->color)
#define stl_rbt_is_black(node) (!stl_rbt_is_red(node))
#define stl_rbt_copy_color(n1, n2) (n1->color = n2->color)

static inline void stl_rbtree_left_rotate(stl_rbtree_node **root, stl_rbtree_node *sentinel,
                                          stl_rbtree_node *node)
{
  stl_rbtree_node *temp;

  temp = node->right;
  node->right = temp->left;

  if (temp->left != sentinel)
  {
    temp->left->parent = node;
  }

  temp->parent = node->parent;

  if (node == *root)
  {
    *root = temp;
  }
  else if (node == node->parent->left)
  {
    node->parent->left = temp;
  }
  else
  {
    node->parent->right = temp;
  }

  temp->left = node;
  node->parent = temp;
}

static inline void
stl_rbtree_right_rotate(stl_rbtree_node **root, stl_rbtree_node *sentinel,
                        stl_rbtree_node *node)
{
  stl_rbtree_node *temp;

  temp = node->left;
  node->left = temp->right;

  if (temp->right != sentinel)
  {
    temp->right->parent = node;
  }

  temp->parent = node->parent;

  if (node == *root)
  {
    *root = temp;
  }
  else if (node == node->parent->right)
  {
    node->parent->right = temp;
  }
  else
  {
    node->parent->left = temp;
  }

  temp->right = node;
  node->parent = temp;
}

stl_rbtree *stl_rbtree_alloc(stl_rbtree_key_cmp cmp)
{
  stl_rbtree *tree = (stl_rbtree *)calloc(1, sizeof(stl_rbtree));
  if (tree != NULL && stl_rbtree_init(tree, cmp) != 0)
  {
    free(tree->sentinel);
    tree->sentinel = NULL;
    free(tree);
    tree = NULL;
  }
  return tree;
}
int stl_rbtree_init(stl_rbtree *tree, stl_rbtree_key_cmp cmp)
{

  if (tree)
  {
    tree->root = NULL;
    tree->sentinel = (stl_rbtree_node *)calloc(1, sizeof(stl_rbtree_node));
    tree->cmp = cmp;
    stl_rbt_black(tree->sentinel);
    return 0;
  }
  return -1;
}

static inline void stl_rbtree_insert_value(stl_rbtree *tree, stl_rbtree_node *temp, stl_rbtree_node *node, stl_rbtree_node *sentinel)
{
  stl_rbtree_node **p;

  for (;;)
  {

    p = (tree->cmp(node->key, temp->key) < 0) ? &temp->left : &temp->right;

    if (*p == sentinel)
    {
      break;
    }

    temp = *p;
  }

  *p = node;
  node->parent = temp;
  node->left = sentinel;
  node->right = sentinel;
  stl_rbt_red(node);
}

int stl_rbtree_insert(stl_rbtree *tree, stl_rbtree_node *node)
{
  stl_rbtree_node **root, *temp, *sentinel;

  /* a binary tree insert */

  root = &tree->root;
  sentinel = tree->sentinel;

  if (*root == sentinel)
  {
    node->parent = NULL;
    node->left = sentinel;
    node->right = sentinel;
    stl_rbt_black(node);
    *root = node;

    return 0;
  }

  stl_rbtree_insert_value(tree, *root, node, sentinel);

  /* re-balance tree */

  while (node != *root && stl_rbt_is_red(node->parent))
  {

    if (node->parent == node->parent->parent->left)
    {
      temp = node->parent->parent->right;

      if (stl_rbt_is_red(temp))
      {
        stl_rbt_black(node->parent);
        stl_rbt_black(temp);
        stl_rbt_red(node->parent->parent);
        node = node->parent->parent;
      }
      else
      {
        if (node == node->parent->right)
        {
          node = node->parent;
          stl_rbtree_left_rotate(root, sentinel, node);
        }

        stl_rbt_black(node->parent);
        stl_rbt_red(node->parent->parent);
        stl_rbtree_right_rotate(root, sentinel, node->parent->parent);
      }
    }
    else
    {
      temp = node->parent->parent->left;

      if (stl_rbt_is_red(temp))
      {
        stl_rbt_black(node->parent);
        stl_rbt_black(temp);
        stl_rbt_red(node->parent->parent);
        node = node->parent->parent;
      }
      else
      {
        if (node == node->parent->left)
        {
          node = node->parent;
          stl_rbtree_right_rotate(root, sentinel, node);
        }

        stl_rbt_black(node->parent);
        stl_rbt_red(node->parent->parent);
        stl_rbtree_left_rotate(root, sentinel, node->parent->parent);
      }
    }
  }

  stl_rbt_black(*root);
  return 0;
}

int stl_rbtree_delete(stl_rbtree *tree, stl_rbtree_node *node)
{
  uintptr_t red;
  stl_rbtree_node **root, *sentinel, *subst, *temp, *w;

  /* a binary tree delete */

  root = &tree->root;
  sentinel = tree->sentinel;

  if (node->left == sentinel)
  {
    temp = node->right;
    subst = node;
  }
  else if (node->right == sentinel)
  {
    temp = node->left;
    subst = node;
  }
  else
  {
    subst = stl_rbtree_min(node->right, sentinel);
    temp = subst->right;
  }

  if (subst == *root)
  {
    *root = temp;
    stl_rbt_black(temp);

    /* DEBUG stuff */
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->key = 0;

    return 0;
  }

  red = stl_rbt_is_red(subst);

  if (subst == subst->parent->left)
  {
    subst->parent->left = temp;
  }
  else
  {
    subst->parent->right = temp;
  }

  if (subst == node)
  {

    temp->parent = subst->parent;
  }
  else
  {

    if (subst->parent == node)
    {
      temp->parent = subst;
    }
    else
    {
      temp->parent = subst->parent;
    }

    subst->left = node->left;
    subst->right = node->right;
    subst->parent = node->parent;
    stl_rbt_copy_color(subst, node);

    if (node == *root)
    {
      *root = subst;
    }
    else
    {
      if (node == node->parent->left)
      {
        node->parent->left = subst;
      }
      else
      {
        node->parent->right = subst;
      }
    }

    if (subst->left != sentinel)
    {
      subst->left->parent = subst;
    }

    if (subst->right != sentinel)
    {
      subst->right->parent = subst;
    }
  }

  /* DEBUG stuff */
  node->left = NULL;
  node->right = NULL;
  node->parent = NULL;
  node->key = 0;

  if (red)
  {
    return;
  }

  /* a delete fixup */

  while (temp != *root && stl_rbt_is_black(temp))
  {

    if (temp == temp->parent->left)
    {
      w = temp->parent->right;

      if (stl_rbt_is_red(w))
      {
        stl_rbt_black(w);
        stl_rbt_red(temp->parent);
        stl_rbtree_left_rotate(root, sentinel, temp->parent);
        w = temp->parent->right;
      }

      if (stl_rbt_is_black(w->left) && stl_rbt_is_black(w->right))
      {
        stl_rbt_red(w);
        temp = temp->parent;
      }
      else
      {
        if (stl_rbt_is_black(w->right))
        {
          stl_rbt_black(w->left);
          stl_rbt_red(w);
          stl_rbtree_right_rotate(root, sentinel, w);
          w = temp->parent->right;
        }

        stl_rbt_copy_color(w, temp->parent);
        stl_rbt_black(temp->parent);
        stl_rbt_black(w->right);
        stl_rbtree_left_rotate(root, sentinel, temp->parent);
        temp = *root;
      }
    }
    else
    {
      w = temp->parent->left;

      if (stl_rbt_is_red(w))
      {
        stl_rbt_black(w);
        stl_rbt_red(temp->parent);
        stl_rbtree_right_rotate(root, sentinel, temp->parent);
        w = temp->parent->left;
      }

      if (stl_rbt_is_black(w->left) && stl_rbt_is_black(w->right))
      {
        stl_rbt_red(w);
        temp = temp->parent;
      }
      else
      {
        if (stl_rbt_is_black(w->left))
        {
          stl_rbt_black(w->right);
          stl_rbt_red(w);
          stl_rbtree_left_rotate(root, sentinel, w);
          w = temp->parent->left;
        }

        stl_rbt_copy_color(w, temp->parent);
        stl_rbt_black(temp->parent);
        stl_rbt_black(w->left);
        stl_rbtree_right_rotate(root, sentinel, temp->parent);
        temp = *root;
      }
    }
  }

  stl_rbt_black(temp);
  return 0;
}

stl_rbtree_node *stl_btree_search(stl_rbtree *tree, void *key)
{
  stl_rbtree_node *node = NULL;
  if (tree)
  {
    stl_rbtree_node *tmp = tree->root;
    while (tmp)
    {
      if (tree->cmp(tmp->key, key) == 0)
      {
        node = tmp;
        break;
      }
      else if (tree->cmp(key, tmp->key) > 0)
      {
        tmp = tmp->right;
      }
      else
      {
        tmp = tmp->left;
      }
    }
  }
  return node;
}
int stl_rbtree_deinit(stl_rbtree *tree)
{
  if(tree) {
    //todo 
  }
  return -1;
}
void stl_rbtree_destroy(stl_rbtree *tree) {
  if(tree && stl_rbtree_deinit(tree)!=-1)
  {
     free(tree);
     tree = NULL;
  }

}
