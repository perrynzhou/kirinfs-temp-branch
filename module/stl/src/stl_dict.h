/*************************************************************************
    > File Name: stl_dict.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 05 Sep 2022 03:16:30 AM UTC
 ************************************************************************/

#ifndef _STL_DICT_H
#define _STL_DICT_H
#include <stdint.h>
#include <pthread.h>
typedef enum
{
  DICT_ITEM_TYPE_INT = 0,
  DICT_ITEM_TYPE_UINT = 1,
  DICT_ITEM_TYPE_DOUBLE = 2,
  DICT_ITEM_TYPE_STR = 3,
  DICT_ITEM_TYPE_PTR = 4,
} stl_dict_item_type;

typedef struct
{
  union
  {
    char *str;
    int64_t i64;
    uint64_t u64;
    double d64;
    void *ptr;
  } data;
  stl_dict_item_type data_type;
  uint32_t len;
} stl_dict_item;

typedef int (*stl_dict_cmp)(stl_dict_item *, stl_dict_item *);
typedef uint32_t (*stl_dict_hash)(void *, size_t);
typedef void (*stl_dict_func)(void *,stl_dict_item *,stl_dict_item *);
typedef struct
{
  uint64_t max_count;
  uint32_t count;
  void **buckets;
  stl_dict_cmp cmp;
  stl_dict_hash hash;
} stl_dict;

stl_dict *stl_dict_alloc(uint32_t size,stl_dict_cmp cmp,stl_dict_hash hash);
int stl_dict_init(stl_dict *dict, uint32_t size,stl_dict_cmp cmp,stl_dict_hash hash);
stl_dict_item *stl_dict_lookup(stl_dict *dict, stl_dict_item *key);
int stl_dict_insert(stl_dict *dict, stl_dict_item *key, stl_dict_item *value);
int stl_dict_delete(stl_dict *dict, stl_dict_item *key);
int stl_dict_dump(stl_dict *dict, void *ctx,stl_dict_func cb);
int stl_dict_deinit(stl_dict *dict);
void stl_dict_destroy(stl_dict *dict);
#endif
