/*************************************************************************
  > File Name: stl_slab.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 12 Sep 2022 12:47:37 AM EDT
 ************************************************************************/

#ifndef _STL_SLAB_H
#define _STL_SLAB_H
#include <stdint.h>
#include <stdio.h>
// abstract from nginx slab,need to deep test
typedef struct stl_slab_page
{
  uintptr_t slab;
  struct stl_slab_page *next;
  uintptr_t prev;
} stl_slab_page;

typedef struct
{
  uintptr_t total;
  uintptr_t used;

  uintptr_t reqs;
  uintptr_t fails;
} stl_slab_stat;

typedef struct
{
  volatile uint64_t lock;
  size_t min_size;
  size_t min_shift;

  stl_slab_page *pages;
  stl_slab_page *last;
  stl_slab_page free;
  stl_slab_stat *stats;
  uintptr_t pfree;

  unsigned char *start;
  unsigned char *end;
  void *addr;
} stl_slab;

int stl_slab_init(stl_slab *pool, size_t pool_size);
void *stl_slab_alloc(stl_slab *pool, size_t size);
void stl_slab_free(stl_slab *pool, void *p);
#endif
