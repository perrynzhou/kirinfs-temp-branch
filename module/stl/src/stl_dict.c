/*************************************************************************
    > File Name: stl_dict.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 05 Sep 2022 03:16:36 AM UTC
 ************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "stl_dict.h"

#define STL_DICT_INIT_SIZE (64)

/*

extern inline bool stl_ring_buffer_is_full(stl_ring_buffer *rb);
extern inline bool stl_ring_buffer_is_empty(stl_ring_buffer *rb);

*/

typedef struct stl_dict_item_pair stl_dict_item_pair;
struct stl_dict_item_pair
{

  stl_dict_item key;
  stl_dict_item value;
  struct stl_dict_item_pair *next;
} ;

inline static bool stl_dict_item_check_type(stl_dict_item *item)
{
  if (item->data_type >= DICT_ITEM_TYPE_INT && item->data_type <= DICT_ITEM_TYPE_PTR)
  {
    return true;
  }
  return false;
}
static inline int stl_dict_item_ptr_alloc(stl_dict_item *dst, stl_dict_item *item)
{
  int ret = 0;
  switch (item->data_type)
  {
  case DICT_ITEM_TYPE_STR:
    dst->data.str = strdup(item->data.str);
    dst->len = strlen(dst->data.str);
    break;
  case DICT_ITEM_TYPE_PTR:
    dst->data.ptr = calloc(dst->len, sizeof(char));
    memcpy(dst->data.ptr, item->data.ptr, item->len);
    break;
  default:
    break;
  }
  return ret;
}
static inline int stl_dict_item_ptr_destroy(stl_dict_item *item)
{
  switch (item->data_type)
  {
  case DICT_ITEM_TYPE_STR:
    free(item->data.str);
    item->data.str = NULL;
    break;
  case DICT_ITEM_TYPE_PTR:
    free(item->data.ptr);
    item->data.ptr = NULL;
    break;
  default:
    break;
  }
}
static stl_dict_item_pair *stl_dict_item_pair_alloc(stl_dict_item *key, stl_dict_item *value)
{
  stl_dict_item_pair *pair = NULL;
  if (key && value)
  {
    pair = calloc(1, sizeof(stl_dict_item_pair));
    if (key->data_type != DICT_ITEM_TYPE_STR || key->data_type != DICT_ITEM_TYPE_PTR)
    {
       memcpy(&pair->key, key, key->len);
    }
    memcpy(&pair->value, value, value->len);
    stl_dict_item_ptr_alloc(&pair->key, key);
    stl_dict_item_ptr_alloc(&pair->value, value);
  }
  return pair;
}

static void stl_dict_item_pair_destroy(stl_dict_item_pair *pair)
{
  if (pair != NULL)
  {
    stl_dict_item_ptr_destroy(&pair->key);
    stl_dict_item_ptr_destroy(&pair->value);
    free(pair);
    pair = NULL;
  }
}
static int stl_dict_item_pair_insert(stl_dict *dict, uint32_t index, stl_dict_item *key, stl_dict_item *value)
{
  if (dict)
  {
    stl_dict_item_pair **buckets = (stl_dict_item_pair **)dict->buckets;
    stl_dict_item_pair *pair = stl_dict_item_pair_alloc(key, value);

    pair->next = buckets[index];
    buckets[index] = pair;
    dict->count++;
    return 0;
  }
  return -1;
}
static int stl_dict_item_pair_delete(stl_dict *dict, uint32_t index, stl_dict_item *key)

{
  if (dict)
  {
    stl_dict_item_pair **buckets = (stl_dict_item_pair **)dict->buckets;
    stl_dict_item_pair *pair = buckets[index];
    stl_dict_item_pair *prev = NULL;
    while (pair != NULL)
    {
      if (dict->cmp(&pair->key, key) == 0)
      {
        break;
      }
      prev = pair;
      pair = pair->next;
    }
    if (pair != NULL)
    {
      if (prev == NULL)
      {
        buckets[index] = NULL;
      }
      else
      {
        prev->next = pair->next;
      }
      dict->count--;
      stl_dict_item_pair_destroy(pair);
    }
    return 0;
  }
  return -1;
}

int stl_dict_init(stl_dict *dict, uint32_t size, stl_dict_cmp cmp, stl_dict_hash hash)
{
  int ret = -1;
  if (dict && size > 0)
  {
    dict->count = 0;
    uint32_t init_size = (size < STL_DICT_INIT_SIZE) ? STL_DICT_INIT_SIZE : size;
    dict->buckets = (void **)calloc(init_size, sizeof(void *));
    if (dict->buckets != NULL)
    {
      ret = 0;
      dict->count = 0;
      dict->max_count = STL_DICT_INIT_SIZE;
      dict->hash = hash;
      dict->cmp = cmp;
    }
    if (ret<0)
    {
      free(dict->buckets);
      dict->buckets = NULL;
    }
  }
  return ret;
}
stl_dict *stl_dict_alloc(uint32_t size, stl_dict_cmp cmp, stl_dict_hash hash)
{
  stl_dict *dict = calloc(1, sizeof(stl_dict));
  if (dict != NULL && stl_dict_init(dict, size, cmp, hash) != 0)
  {
    free(dict);
    dict = NULL;
  }
  return dict;
}
int stl_dict_insert(stl_dict *dict, stl_dict_item *key, stl_dict_item *value)
{
  int ret = -1;
  if (dict && key && value && stl_dict_item_check_type(key) && stl_dict_item_check_type(value))
  {
    uint32_t index = dict->hash(key, key->len) % dict->max_count;
    ret = stl_dict_item_pair_insert(dict, index, key, value);
  }
  return ret;
}
int stl_dict_delete(stl_dict *dict, stl_dict_item *key)
{
  int ret = -1;
  if (dict && key && stl_dict_item_check_type(key))
  {
    uint32_t index = dict->hash(key, key->len) % dict->max_count;
    ret = stl_dict_item_pair_delete(dict, index, key);
  }
  return ret;
}
stl_dict_item *stl_dict_lookup(stl_dict *dict, stl_dict_item *key)
{
  stl_dict_item *target = NULL;
  if (dict && key && stl_dict_item_check_type(key))
  {
    uint32_t index = dict->hash(key, key->len) % dict->max_count;
    stl_dict_item_pair *head = dict->buckets[index];
    while (head != NULL)
    {
      if (dict->cmp(&head->key, key) == 0)
      {
        target = &head->value;
        break;
      }
      head = head->next;
    }
  }
  return target;
}
int stl_dict_dump(stl_dict *dict, void *ctx, stl_dict_func cb)
{
  if (dict && cb)
  {
    for (uint32_t i = 0; i < dict->max_count; i++)
    {
      if (dict->buckets[i] != NULL)
      {
    
      stl_dict_item_pair *head = dict->buckets[i];
      while (head != NULL)
      {

        cb(ctx, &head->key, &head->value);
        head = head->next;
      }
      }
    }
  }
}
int stl_dict_deinit(stl_dict *dict) {
   int ret = -1;
   if(dict) {
        for (uint32_t i = 0; i < dict->max_count; i++)
    {
      if (dict->buckets[i] == NULL)
      {
        continue;
      }
      stl_dict_item_pair *head = dict->buckets[i];
      while (head != NULL)
      {
        stl_dict_item_pair_destroy(head);
        head = head->next;
      }
      dict->count =0;
      free(dict->buckets);
      dict->max_count = 0;
      ret = 0;
    }
   }
   return ret;
}
void stl_dict_destroy(stl_dict *dict) {
  if(dict) {
    stl_dict_deinit(dict);
    free(dict);
    dict = NULL;
  }
}
