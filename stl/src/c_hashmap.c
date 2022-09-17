/*************************************************************************
    > File Name: c_hash_map.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Tue 22 Mar 2022 08:21:44 AM UTC
 ************************************************************************/

#ifndef _C_HASH_MAP_H
#define _C_HASH_MAP_H
#include "inc/c_hashmap.h"
typedef struct
{
  void *key;
  void *val;
  void *next;
} c_member_pair_t;

static c_member_pair_t *c_member_pair_alloc(void *key, size_t key_sz, void *val, size_t val_sz)
{
  c_member_pair_t *pair = NULL;
  if (key != NULL && val != NULL && key_sz > 0 && val_sz > 0)
  {
    pair = (c_member_pair_t *)calloc(1, sizeof(c_member_pair_t));
    assert(pair != NULL);
    pair->next = NULL;
    pair->key = calloc(1, sizeof(char) * key_sz);
    assert(pair->key != NULL);
    pair->val = calloc(1, sizeof(char) * val_sz);
    assert(pair->val != NULL);
    memcpy(pair->key, key, key_sz);
    memcpy(pair->val, key, val_sz);
  }
  return pair;
}
static uint64_t c_hashmap_jump_consistent(uint64_t key, size_t num_buckets)
{

  int64_t b = -1, j = 0;
  size_t value = 0;
  uint64_t num_bucket = (uint64_t)num_buckets;
  while (j < num_bucket)
  {
    b = j;
    key = key * 2862933555777941757ULL + 1;
    j = (int64_t)((b + 1) * ((double)(1LL << 31) / (double)((key >> 33) + 1)));
  }
  value = (b < 0) ? (~b + 1) : b;
  return value;
}
inline static c_member_pair_t *c_hashmap_search(c_hashmap_t *d, uint32_t index, void *key, size_t key_sz)
{
  c_member_pair_t *tmp = (c_member_pair_t *)d->ptr[index];
  c_member_pair_t *dst = NULL;

  while (tmp != NULL)
  {
    c_member_pair_t *next = (c_member_pair_t *)tmp->next;
    if (d->cmp_cb(tmp->key, key, key_sz) == 0)
    {
      dst = tmp;
      break;
    }
    tmp = next;
  }
  return dst;
}

inline static uint32_t c_hashmap_get_index(c_hashmap_t *d,void *key,size_t key_sz) {
     uint32_t hash_key = d->hash_cb(key, key_sz);
    uint32_t index = c_hashmap_jump_consistent(hash_key, d->max_count);
    return index;
}
inline void c_hashmap_set_free_cb(c_hashmap_t *d, c_hashmap_free_cb key_free_cb, c_hashmap_free_cb val_free_cb)
{
  if (d != NULL)
  {
    d->key_free_cb = key_free_cb;
    d->val_free_cb = val_free_cb;
  }
}
static void c_member_pair_free(c_member_pair_t *pair, c_hashmap_free_cb key_free, c_hashmap_free_cb val_free)
{
  if (pair != NULL)
  {
    if (key_free != NULL)
    {
      key_free(pair->key);
      pair->key = NULL;
    }
    if (val_free != NULL)
    {
      val_free(pair->val);
      pair->val = NULL;
    }
    free(pair);
    pair = NULL;
  }
}

int c_hashmap_init(c_hashmap_t *d, size_t max_count, c_hashmap_hash_cb hash_cb, c_hashmap_key_compare_cb cmp_cb)
{
  if (d != NULL && max_count > 0 && hash_cb != NULL && cmp_cb != NULL)
  {
    d->ptr = (void **)calloc(max_count, sizeof(void **));
    assert(d->ptr != NULL);
    d->hash_cb = hash_cb;
    d->cmp_cb = cmp_cb;
    d->cur_count = 0;
    d->key_free_cb = NULL;
    d->val_free_cb = NULL;

    return 0;
  }
  return -1;
}
c_hashmap_t *c_hashmap_alloc(size_t max_count, c_hashmap_hash_cb hash_cb, c_hashmap_key_compare_cb cmp_cb)
{
  c_hashmap_t *hash_map = NULL;
  if (max_count > 0 && hash_cb != NULL && cmp_cb != NULL)
  {
    hash_map = (c_hashmap_t *)calloc(1, sizeof(c_hashmap_t));
    assert(hash_map != NULL);
    if (c_hashmap_init(hash_map, max_count, hash_cb, cmp_cb) != 0)
    {
      free(hash_map);
      hash_map = NULL;
    }
  }
  return hash_map;
}
int c_hashmap_put(c_hashmap_t *d, void *key, size_t key_sz, void *value, size_t value_sz)
{
  if (d != NULL && key != NULL && key_sz > 0 && value != NULL && value_sz > 0)
  {
    if (d->cur_count < d->max_count)
    {
      void *key_data = calloc(1, sizeof(char *) * key_sz);
      assert(key_data != NULL);
      void *value_data = calloc(1, sizeof(char *) * value_sz);
      assert(value_data != NULL);
      c_member_pair_t *pair = c_member_pair_alloc(key, key_sz, value, value_sz);
      pair->key = key_data;
      pair->val = value_data;
      uint32_t index = c_hashmap_get_index(d,key,key_sz);
      if (d->ptr[index] == NULL)
      {
        d->ptr[index] = pair;
      }
      else
      {
        pair->next = d->ptr[index];
        d->ptr[index] = pair;
      }
      d->cur_count++;
      return 0;
    }
  }
  return -1;
}

void *c_hashmap_get(c_hashmap_t *d, void *key, size_t key_sz)
{
  void *data = NULL;
  if (d != NULL && d->cur_count > 0)
  {
   uint32_t index = c_hashmap_get_index(d,key,key_sz);
    if (d->ptr[index] != NULL)
    {
      c_member_pair_t *dst = c_hashmap_search(d, index, key, key_sz);
      if (dst != NULL)
      {
        data = dst->val;
      }
    }
  }
  return data;
}
int c_hashmap_del(c_hashmap_t *d, void *key, size_t key_sz)
{
  if (d != NULL && key != NULL)
  {
    c_member_pair_t *dst = NULL;
   uint32_t index = c_hashmap_get_index(d,key,key_sz);
    if (d->ptr[index] != NULL)
    {
      c_member_pair_t *tmp = (c_member_pair_t *)d->ptr[index];
      c_member_pair_t *prev = NULL;
      while (tmp != NULL)
      {
        c_member_pair_t *next = (c_member_pair_t *)tmp->next;
        if (d->cmp_cb(tmp->key, key, key_sz) == 0)
        {
          dst = tmp;
          break;
        }
        prev = tmp;
        tmp = next;
      }

      if (dst != NULL)
      {
        if (prev == NULL)
        {
          d->ptr[index] = dst->next;
        }
        else
        {
          prev->next = dst->next;
        }
        c_member_pair_free(dst, d->key_free_cb, d->val_free_cb);
         d->cur_count--;
      }
    }
    return 0;
  }
  return -1;
}
int c_hashmap_traverse(c_hashmap_t *d,c_hashmap_traverse_cb traverse_cb)
{
   if(d !=NULL && d->cur_count>0)
   {
     for(size_t i=0;i<d->max_count;i++)
     {
        c_member_pair_t *tmp = (c_member_pair_t *)d->ptr[i];
        while(tmp !=NULL)
        {
           c_member_pair_t *next = (c_member_pair_t *)tmp->next;
           if(traverse_cb !=NULL) {
              traverse_cb(tmp->key,tmp->val);
           }
           tmp =next;
        }
     }
     return 0;
   }
   return -1;
}
int c_hashmap_deinit(c_hashmap_t *d)
{
   if(d !=NULL && d->cur_count>0)
   {
     for(size_t i=0;i<d->max_count;i++)
     {
        c_member_pair_t *tmp = (c_member_pair_t *)d->ptr[i];
        while(tmp !=NULL)
        {
           c_member_pair_t *next = (c_member_pair_t *)tmp->next;
           c_member_pair_free(tmp,d->key_free_cb,d->val_free_cb);
           tmp =next;
        }
     }
     return 0;
     
   }
   return -1;
}
void c_hashmap_destroy(c_hashmap_t *d)
{
  if(c_hashmap_deinit(d)!=-1)
  {
    free(d);
    d=NULL;
  }
}
#endif
