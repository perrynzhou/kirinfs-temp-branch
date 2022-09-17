/*************************************************************************
    > File Name: stl_radix_tree.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Tue 13 Sep 2022 03:30:24 AM UTC
 ************************************************************************/

#include <stdio.h>
#include "stl_radix_tree.h"



#define STL_RADIX_NO_VALUE   (uintptr_t) -1


static stl_radix_node *stl_radix_alloc(stl_radix_tree *tree);


stl_radix_tree *
stl_radix_tree_alloc(intptr_t preallocate)
{
    uint32_t           key, mask, inc;
    stl_radix_tree  *tree = calloc(1, sizeof(stl_radix_tree));
    if (tree == NULL) {
        return NULL;
    }
    tree->free = NULL;
    tree->start = NULL;
    tree->size = 0;

    tree->root = stl_radix_alloc(tree);
    if (tree->root == NULL) {
        return NULL;
    }

    tree->root->right = NULL;
    tree->root->left = NULL;
    tree->root->parent = NULL;
    tree->root->value = STL_RADIX_NO_VALUE;

    if (preallocate == 0) {
        return tree;
    }

    /*
     * Preallocation of first nodes : 0, 1, 00, 01, 10, 11, 000, 001, etc.
     * increases TLB hits even if for first lookup iterations.
     * On 32-bit platforms the 7 preallocated bits takes continuous 4K,
     * 8 - 8K, 9 - 16K, etc.  On 64-bit platforms the 6 preallocated bits
     * takes continuous 4K, 7 - 8K, 8 - 16K, etc.  There is no sense to
     * to preallocate more than one page, because further preallocation
     * distributes the only bit per page.  Instead, a random insertion
     * may distribute several bits per page.
     *
     * Thus, by default we preallocate maximum
     *     6 bits on amd64 (64-bit platform and 4K pages)
     *     7 bits on i386 (32-bit platform and 4K pages)
     *     7 bits on sparc64 in 64-bit mode (8K pages)
     *     8 bits on sparc64 in 32-bit mode (8K pages)
     */

    int  stl_pagesize = getpagesize();
    if (preallocate == -1) {
        switch (stl_pagesize / sizeof(stl_radix_node)) {

        /* amd64 */
        case 128:
            preallocate = 6;
            break;

        /* i386, sparc64 */
        case 256:
            preallocate = 7;
            break;

        /* sparc64 in 32-bit mode */
        default:
            preallocate = 8;
        }
    }

    mask = 0;
    inc = 0x80000000;

    while (preallocate--) {

        key = 0;
        mask >>= 1;
        mask |= 0x80000000;

        do {
            if (stl_radix32tree_insert(tree, key, mask, STL_RADIX_NO_VALUE)
                != 0)
            {
                return NULL;
            }

            key += inc;

        } while (key);

        inc >>= 1;
    }

    return tree;
}


intptr_t
stl_radix32tree_insert(stl_radix_tree *tree, uint32_t key, uint32_t mask,
    uintptr_t value)
{
    uint32_t           bit;
    stl_radix_node  *node, *next;

    bit = 0x80000000;

    node = tree->root;
    next = tree->root;

    while (bit & mask) {
        if (key & bit) {
            next = node->right;

        } else {
            next = node->left;
        }

        if (next == NULL) {
            break;
        }

        bit >>= 1;
        node = next;
    }

    if (next) {
        if (node->value != STL_RADIX_NO_VALUE) {
            return -3;
        }

        node->value = value;
        return 0;
    }

    while (bit & mask) {
        next = stl_radix_alloc(tree);
        if (next == NULL) {
            return -1;
        }

        next->right = NULL;
        next->left = NULL;
        next->parent = node;
        next->value = STL_RADIX_NO_VALUE;

        if (key & bit) {
            node->right = next;

        } else {
            node->left = next;
        }

        bit >>= 1;
        node = next;
    }

    node->value = value;

    return 0;
}


intptr_t
stl_radix32tree_delete(stl_radix_tree *tree, uint32_t key, uint32_t mask)
{
    uint32_t           bit;
    stl_radix_node  *node;

    bit = 0x80000000;
    node = tree->root;

    while (node && (bit & mask)) {
        if (key & bit) {
            node = node->right;

        } else {
            node = node->left;
        }

        bit >>= 1;
    }

    if (node == NULL) {
        return -1;
    }

    if (node->right || node->left) {
        if (node->value != STL_RADIX_NO_VALUE) {
            node->value = STL_RADIX_NO_VALUE;
            return 0;
        }

        return -1;
    }

    for ( ;; ) {
        if (node->parent->right == node) {
            node->parent->right = NULL;

        } else {
            node->parent->left = NULL;
        }

        node->right = tree->free;
        tree->free = node;

        node = node->parent;

        if (node->right || node->left) {
            break;
        }

        if (node->value != STL_RADIX_NO_VALUE) {
            break;
        }

        if (node->parent == NULL) {
            break;
        }
    }

    return 0;
}


uintptr_t
stl_radix32tree_find(stl_radix_tree *tree, uint32_t key)
{
    uint32_t           bit;
    uintptr_t          value;
    stl_radix_node  *node;

    bit = 0x80000000;
    value = STL_RADIX_NO_VALUE;
    node = tree->root;

    while (node) {
        if (node->value != STL_RADIX_NO_VALUE) {
            value = node->value;
        }

        if (key & bit) {
            node = node->right;

        } else {
            node = node->left;
        }

        bit >>= 1;
    }

    return value;
}

