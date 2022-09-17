/*************************************************************************
    > File Name: test_stl_dict.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: 一  9/ 5 19:46:35 2022
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include "../src/stl_dict.h"
#include "../src/stl_hash.h"

inline static int dict_cmp(stl_dict_item *a, stl_dict_item *b)
{
   return 0;
}
inline static uint32_t dict_hash(void *a, size_t *b)
{
   const char *data = (char *)a;
   return stl_hash_crc32(data, strlen(data));
}

static inline void dict_dump(void *ctx, stl_dict_item *a, stl_dict_item *b)
{
   if (a && b)
   {
      fprintf(stdout, "dict_dump:key=%ld,value=%s\n", a->data.i64, b->data.str);
   }
}
int main(int argc, char *argv[])
{
   stl_dict dict;
   stl_dict_init(&dict, 16, (stl_dict_cmp)&dict_cmp, (stl_dict_hash)&dict_hash);
   size_t n = 5;
   for (size_t i = 0; i < n; i++)
   {
      stl_dict_item key = {
          .data.i64 = i,
          .data_type = DICT_ITEM_TYPE_INT,
          .len = sizeof(int),
      };
      char buf[64] = {'\0'};
      snprintf((char *)&buf, 64, "self-key-%d", i);
      stl_dict_item value = {
          .data.str = (char *)&buf,
          .data_type = DICT_ITEM_TYPE_STR,
          .len = strlen((char *)&buf),
      };
      int ret = stl_dict_insert(&dict, &key, &value);
      fprintf(stdout, "stl_dict_insert ret=%d\n", ret);
   }
   fprintf(stdout, "************\n");
   for (size_t i = 0; i < n - 3; i++)
   {
      stl_dict_item key = {
          .data.i64 = i,
          .data_type = DICT_ITEM_TYPE_INT,
          .len = sizeof(int),
      };
      stl_dict_item *item = stl_dict_lookup(&dict, &key);
      if (item)
      {
         fprintf(stdout, "stl_dict_lookup:key=%d,val=%s\n", i, item->data.str);
      }
   }
     fprintf(stdout, "************\n");
   stl_dict_item key = {
       .data.i64 = 3,
       .data_type = DICT_ITEM_TYPE_INT,
       .len = sizeof(int),
   };
   int ret =stl_dict_delete(&dict, &key);
   fprintf(stdout, "stl_dict_delete:key=%d,ret=%d\n", key.data.i64, ret);

   stl_dict_dump(&dict,NULL, (stl_dict_func)&dict_dump);

   stl_dict_deinit(&dict);
}