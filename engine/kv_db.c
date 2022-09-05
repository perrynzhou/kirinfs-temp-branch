/*************************************************************************
    > File Name: kv_db.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sun 20 Feb 2022 05:15:21 AM UTC
 ************************************************************************/

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "kv_db.h"
#include "hashfn.h"
#include "log.h"
const char *schema_format = "key_format=u,value_format=u";

int kv_db_register_schema(kv_db_t *db, kv_schema_t *schema);
void kv_db_unregister_schema(kv_db_t *db, kv_schema_t *schema);
inline static void wt_item_init(WT_ITEM *item, void *data, size_t size)
{
  item->data = data;
  item->size = size;
}

kv_schema_t *kv_schema_alloc(const char *schema_name, void *ctx, bool is_force_drop)
{
  assert(ctx != NULL);
  kv_db_t *db = (kv_db_t *)ctx;
  size_t schema_sz = strlen(schema_name);

  kv_schema_t *schema = (kv_schema_t *)malloc(sizeof(kv_schema_t));
  assert(schema != NULL);
  assert(db->conn->open_session(db->conn, NULL, NULL, &schema->session) != -1);
  char schema_buf[256] = {'\0'};
  snprintf((char *)&schema_buf, 256, "table:%s", schema_name);

  if (is_force_drop)
  {
    assert(schema->session->drop(schema->session, (char *)&schema_buf, schema_format) != -1);
  }
  assert(schema->session->create(schema->session, (char *)&schema_buf, schema_format) != -1);
  assert(schema->session->open_cursor(schema->session, (char *)&schema_buf, NULL, NULL, &schema->cursor) != -1);
  schema->uri = strdup((char *)&schema_buf);
  schema->name = strdup(schema_name);
  schema->ctx = ctx;
  fprintf(stdout, "create::schema %s succ\n", schema->uri);
  kv_db_register_schema(db, schema);
  return schema;
}
void kv_schema_destroy(kv_schema_t *schema)
{
  if (schema != NULL)
  {
    kv_db_t *db = (kv_db_t *)schema->ctx;
    kv_db_unregister_schema(db, schema);
    schema->cursor->close(schema->cursor);
    schema->session->drop(schema->session, schema->uri, "remove_files=true");
    schema->session->close(schema->session, NULL);
    free(schema->uri);
    free(schema->name);
    free(schema);
    schema = NULL;
  }
}

kv_db_t *kv_db_alloc(const char *database_name, const char *database_dir)
{
  if (database_name != NULL && database_dir != NULL)
  {
    char base_path[2048] = {'\0'};
    snprintf((char *)&base_path, 2048, "%s/%s", database_dir, database_name);
    if (access((char *)&base_path, F_OK) != 0 && mkdir((char *)&base_path, 0755) != 0)
    {

      return NULL;
    }
    kv_db_t *db = (kv_db_t *)calloc(1, sizeof(kv_db_t));
    assert(db != NULL);
    char *conn_config = "create,cache_size=4GB,session_max=50000,eviction=(threads_min=4,threads_max=8),log=(enabled=false),transaction_sync=(enabled=false),checkpoint_sync=true,checkpoint=(wait=10),statistics=(fast),statistics_log=(json,wait=1)";
    assert(wiredtiger_open((char *)&base_path, NULL, conn_config, &db->conn) != -1);
    db->database_name = strdup(database_name);
    db->database_dir = strdup(database_dir);
    db->schema_ctx = dict_create(SCHEMA_LIMIT, hash_fnv1_64);
    pthread_mutex_init(&db->lock, NULL);
    return db;
  }
  return NULL;
}
int kv_db_set(kv_db_t *db, char *schema_name, void *key, size_t key_sz, void *val, size_t val_sz)
{

  kv_schema_t *schema = (kv_schema_t *)dict_get(db->schema_ctx, schema_name);
  if (schema != NULL)
  {

    WT_CURSOR *cursor = schema->cursor;
    WT_ITEM key_item, value_item;
    wt_item_init(&key_item, key, key_sz);
    wt_item_init(&value_item, val, val_sz);
    cursor->set_key(cursor, &key_item);
    cursor->set_value(cursor, &value_item);
    return cursor->insert(cursor);
  }
  return -1;
}

int kv_db_search(kv_db_t *db, char *schmea_name, void *key, size_t key_sz)
{
  kv_schema_t *schema = (kv_schema_t *)dict_get(db->schema_ctx, schmea_name);
  int ret = -1;
  if (schema != NULL)
  {
    WT_CURSOR *cursor = schema->cursor;
    WT_ITEM key_item, value_item;
    wt_item_init(&key_item, key, key_sz);
    cursor->set_key(cursor, &key_item);
    ret = cursor->search(cursor);
  }
  return ret;
}
void *kv_db_get(kv_db_t *db, char *schema_name, void *key, size_t key_sz, size_t *value_size)
{
  kv_schema_t *schema = (kv_schema_t *)dict_get(db->schema_ctx, schema_name);
  if (schema != NULL)
  {
    WT_CURSOR *cursor = schema->cursor;
    WT_ITEM key_item, value_item;
    wt_item_init(&key_item, key, key_sz);
    cursor->set_key(cursor, &key_item);
    if (cursor->search(cursor) != 0)
    {
      return NULL;
    }
    cursor->get_value(cursor, &value_item);
    if (value_size != NULL)
    {
      *value_size = value_item.size;
    }
    return (void *)value_item.data;
  }
  return NULL;
}
int kv_db_del(kv_db_t *db, char *schema_name, void *key, size_t key_sz)
{
  kv_schema_t *schema = (kv_schema_t *)dict_get(db->schema_ctx, schema_name);
  WT_CURSOR *cursor = schema->cursor;
  WT_ITEM key_item;
  wt_item_init(&key_item, key, key_sz);
  cursor->set_key(cursor, &key_item);
  return cursor->remove(cursor);
}
inline static void kv_schema_free_cb(void *ptr)
{
  kv_schema_t *schema = (kv_schema_t *)ptr;
  kv_schema_destroy(schema);
}
void *kv_db_destroy(kv_db_t *db)
{
  if (db != NULL)
  {

    dict_deinit(db->schema_ctx, kv_schema_free_cb);
    free(db->database_dir);
    free(db->database_name);
  }
}
kv_schema_t *kv_db_fetch_schema(kv_db_t *db, char *schema_name)
{
  if (db == NULL || schema_name == NULL)
  {
    return NULL;
  }
  return (kv_schema_t *)dict_get(db->schema_ctx, schema_name);
}
int kv_db_register_schema(kv_db_t *db, kv_schema_t *schema)
{
  if (db == NULL || schema == NULL)
  {
    return -1;
  }
  char *schema_name = schema->name;
  if (dict_put(db->schema_ctx, schema_name, schema) != NULL)
  {
    return 0;
  }

  return -1;
}
void kv_db_unregister_schema(kv_db_t *db, kv_schema_t *schema)
{
  if (db != NULL && schema != NULL && schema->name != NULL)
  {
    dict_del(db->schema_ctx, schema->name, NULL);
  }
}
int kv_schmea_search(kv_schema_t *schema, kv_schmea_func_cb cb, void *ctx, void *dst_key, size_t dst_key_sz)
{

  WT_SESSION *session = schema->session;
  WT_CURSOR *cursor = NULL;
  session->open_cursor(session, schema->uri, NULL, NULL, &cursor);
  int ret = -1;
  while ((ret = cursor->next(cursor)) == 0)
  {
    WT_ITEM key;
    WT_ITEM val;
    ret = cursor->get_key(cursor, &key);
    if (ret != 0)
    {
      return -1;
    }
    ret = cursor->get_value(cursor, &val);
    char *key_str = (char *)key.data;
    key_str[key.size] = '\0';
    log_info("search key=%s,len=%d", key_str, key.size);
    if (dst_key != NULL && dst_key_sz > 0 && memcmp(key_str, dst_key, dst_key_sz))
    {
      cursor->close(cursor);
      return cb(ctx, key_str, (void *)val.data);
    }
    else
    {
      cb(ctx, key_str, (void *)val.data);
    }
  }
  cursor->close(cursor);
  return -1;
}