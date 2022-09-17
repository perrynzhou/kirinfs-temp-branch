/*************************************************************************
    > File Name: c_hash_map.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Tue 22 Mar 2022 08:21:59 AM UTC
 ************************************************************************/

#ifndef _C_HASHMAP_H
#define _C_HASHMAP_H
#include "c_utils.h"

typedef struct
{
  size_t max_count;
  size_t cur_count;
  void **ptr;
  c_hashmap_hash_cb hash_cb;
  c_hashmap_key_compare_cb cmp_cb;
  c_hashmap_free_cb key_free_cb;
  c_hashmap_free_cb val_free_cb;
} c_hashmap_t;

void c_hashmap_set_free_cb( c_hashmap_t *d,c_hashmap_free_cb key_free_cb,c_hashmap_free_cb val_free_cb);
int c_hashmap_init(c_hashmap_t *d, size_t max_count, c_hashmap_hash_cb hash_cb,c_hashmap_key_compare_cb cmp_cb);
c_hashmap_t *c_hashmap_alloc(size_t max_count,c_hashmap_hash_cb hash_cb,c_hashmap_key_compare_cb cmp_cb);
//void *c_hashmap_put(c_hashmap_t *d, char *key, void *val);

int c_hashmap_put(c_hashmap_t *d, void *key,size_t key_sz, void *value,size_t value_sz);
void *c_hashmap_get(c_hashmap_t *d, void *key,size_t key_sz);
int c_hashmap_del(c_hashmap_t *d, void *key,size_t key_sz);
int c_hashmap_traverse(c_hashmap_t *d,c_hashmap_traverse_cb traverse_cb);
int c_hashmap_deinit(c_hashmap_t *d);
void c_hashmap_destroy(c_hashmap_t *d);

inline static size_t c_hashmap_count(c_hashmap_t *d) {
  if(d!=NULL) {
    return d->cur_count;
  }
  return -1;
}

inline static bool c_hashmap_empty(c_hashmap_t *d) {
  if(d->cur_count <=0) {
    return false;
  }
  return true;
}
#endif
