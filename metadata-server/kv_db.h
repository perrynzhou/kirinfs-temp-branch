/*************************************************************************
    > File Name: kv_db.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sun 20 Feb 2022 05:10:58 AM UTC
 ************************************************************************/

#ifndef _KV_DB_H
#define _KV_DB_H
#include <stdio.h>
#include <wiredtiger_ext.h>
#include <wiredtiger.h>
#include <pthread.h>
#include "dict.h"
#define SCHEMA_ENTRIES 0
#define SCHEMA_DOCS 1
#define SCHEMA_STATE 2

#define SCHEMA_LIMIT (1024)

typedef struct
{

  WT_SESSION *session;
  WT_CURSOR *cursor;
  void *ctx;
  char *uri;
  char  *name;
} kv_schema_t;

typedef struct
{
  char *database_name;
  char *database_dir;
  dict_t *schema_ctx;
  WT_CONNECTION *conn;
  // struct kv_schema **schema_ctx;
  dict_t *schmea_meta_cache;
  pthread_mutex_t lock;
} kv_db_t;

typedef int (*kv_schmea_func_cb)(void *ctx, void *key, void *val);
kv_schema_t *kv_schema_alloc(const char *name, void *ctx, bool is_force_drop);
int kv_schmea_search(kv_schema_t *schema, kv_schmea_func_cb cb, void *ctx,void *dst_key, size_t dst_key_sz);
void kv_schema_destroy(kv_schema_t *schema);

kv_db_t *kv_db_alloc(const char *database_name, const char *database_dir);
kv_schema_t *kv_db_fetch_schema(kv_db_t *db, char *schema_name);
// key and value operation
int kv_db_search(kv_db_t *db, char *schmea_name,void *key,size_t key_sz);
int kv_db_set(kv_db_t *db, char *schema_name, void *key, size_t key_sz, void *val, size_t val_sz);
void *kv_db_get(kv_db_t *db, char *schema_name, void *key, size_t key_sz,size_t *value_size);
int kv_db_del(kv_db_t *db, char *schema_name, void *key, size_t key_sz);
void *kv_db_destroy(kv_db_t *db);
#endif
