/*************************************************************************
  > File Name: stl_string.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Tue 22 Mar 2022 07:37:01 AM UTC
 ************************************************************************/

#include "stl_string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#define STL_STRING_TEMP_BUFFER_SIZE (2048)
#define STL_STRING_SPLIT_CNT (256)
void stl_string_reset(stl_string *c)
{
  if (c)
  {
    if (c->len >= STL_STRING_INLINE_LEN)
    {
      free(c->data.ptr);
      c->data.ptr = NULL;
    }
    c->len = 0;
  }
}
size_t stl_string_len(stl_string *c)
{
  if (c != NULL)
  {
    return c->len;
  }
  return -1;
}
char *stl_string_data(stl_string *c)
{
  if (c != NULL)
  {
    return (c->len < STL_STRING_INLINE_LEN) ? ((char *)&c->data.in_data) : (c->data.ptr);
  }
  return NULL;
}
int stl_string_split(stl_string *src, const char *delim, stl_string ***c_ptr, size_t *cnt)
{
  int ret = -1;
  if (src && delim && c_ptr && cnt)
  {
    char *string = stl_string_data(src);
    char *found = NULL;
    stl_string **tmp = (stl_string **)calloc(STL_STRING_SPLIT_CNT, sizeof(stl_string *));
    size_t i = 0;
    while ((found = strsep(&string, delim)) != NULL)
    {
      tmp[i++] = stl_string_alloc(found);
    }
    if (i > 0)
    {
      **c_ptr = calloc(i,sizeof(stl_string **));
      *cnt = i;

      for (size_t j = 0; j < i; j++)
      {
        *c_ptr[j] = tmp[j];
      }
    }

    if (tmp)
    {
      free(tmp);
      tmp = NULL;
    }
  }
  return ret;
}
int stl_string_replace(stl_string *src, const char *old_data, const char *new_data)
{
  int ret = -1;
  if (src && old_data && new_data)
  {

    size_t len = stl_string_len(src);
    size_t i = 0;
    size_t old_len = strlen(old_data);
    size_t new_len = strlen(new_data);
    if (len < old_len)
    {
      return ret;
    }
    char *data = stl_string_data(src);
    bool done = false;
    for (; i < len; i++)
    {
      if (data[i] == old_data[0] && strncmp((char *)&data[i], old_data, old_len) == 0)
      {
        done = true;
        break;
      }
    }
    if (done)
    {
      size_t target_len = i + new_len;
      if (target_len < STL_STRING_INLINE_LEN)
      {
        if (len >= STL_STRING_INLINE_LEN)
        {
          free(src->data.ptr);
          src->data.ptr = NULL;
        }
        strncpy(data + i, new_data, new_len);
        src->data.in_data[target_len] = '\0';
      }
      else
      {
        char *ptr = realloc(src->data.ptr, target_len + 1);
        src->data.ptr = ptr;
        strncpy(data + i, new_data, new_len);
        src->data.ptr[target_len] = '\0';
      }
      ret = 0;
    }
  }
  return ret;
  // todo
}
void stl_string_trim(stl_string *src)
{
  if (src)
  {

    size_t len = stl_string_len(src);
    char *data = stl_string_data(src);
    int i = 0, j = 0;
    for (; i < len; i++)
    {
      if (data[i] != ' ')
      {
        data[j++] = data[i];
        fprintf(stdout, "data[%d]=%c\n", j, data[j - 1]);
      }
    }

    if (j > 0)
    {
      if (j < STL_STRING_INLINE_LEN)
      {
        char *ptr = data;
        src->data.ptr = NULL;
        memmove(&src->data.in_data, ptr, j);
        src->data.in_data[j] = '\0';
        free(ptr);
      }
      src->len = j;
    }
  }
}
int stl_string_dup(stl_string *src, stl_string *dst)
{
  int ret = -1;
  if (src && dst)
  {
    const char *src_data = stl_string_data(src);
    size_t src_len = stl_string_len(src);
    char *ptr = NULL;
    if (src_len < STL_STRING_INLINE_LEN)
    {
      ptr = (char *)&dst->data.in_data;
    }
    else
    {
      ptr = calloc(src_len + 1, sizeof(char));
      assert(dst->data.ptr != NULL);
    }
    strncpy((char *)&dst->data.in_data, src_data, src_len);
  }
  return ret;
}
stl_string *stl_string_alloc(char *data)
{
  stl_string *c = NULL;
  if (data != NULL)
  {
    c = (stl_string *)calloc(1, sizeof(stl_string));
    assert(c != NULL);
    stl_string_init(c, data);
  }
  return c;
}
stl_string *stl_string_empty(size_t size)
{
  stl_string *ptr = NULL;
  ptr = (stl_string *)calloc(1, sizeof(stl_string));
  assert(ptr != NULL);
  return ptr;
}
int stl_string_append(stl_string *c, char *str)
{
  if (c != NULL && str != NULL)
  {
    size_t sz = strlen(str);
    size_t len = c->len;
    c->len += sz;
    char *data_ptr = stl_string_data(c);
    if (c->len < STL_STRING_INLINE_LEN)
    {
      strncpy(data_ptr + len, str, sz);
      return 0;
    }
    char *base = (len < STL_STRING_INLINE_LEN) ? ((char *)calloc(c->len + 1, sizeof(char))) : ((char *)realloc(c->data.ptr, (c->len + 1) * sizeof(char)));
    assert(base != NULL);
    strncpy(base, data_ptr, len);
    strncpy(base + len, str, sz);
    c->data.ptr = base;
    return 0;
  }
  return -1;
}
// common for hash_prefix or hash_subffix
bool stl_string_has_strfix(stl_string *c, char *s, bool direct)
{
  if (c && s)
  {

    char *ptr_dst = NULL;
    size_t len = strlen(s);
    if (len > c->len)
    {
      return false;
    }
    char *data = stl_string_data(c);
    if (direct)
    {
      return strncmp(data + (c->len - len), ptr_dst, len) == 0 ? true : false;
    }
    return strncmp(data, ptr_dst, len) == 0 ? true : false;
  }
  return false;
}

bool stl_string_hassuffix(stl_string *c, char *data)
{

  return stl_string_has_strfix(c, data, true);
}
bool stl_string_hasprefix(stl_string *c, char *data)
{
  return stl_string_has_strfix(c, data, false);
}
int stl_string_init(stl_string *c, char *data)
{
  if (c != NULL && data != NULL)
  {

    char *src_ptr = (char *)data;
    size_t len = strlen(src_ptr);
    char *base_ptr = (char *)&c->data.in_data;
    if (len >= STL_STRING_INLINE_LEN)
    {
      c->data.ptr = calloc(len + 1, sizeof(char));
      assert(base_ptr);
      base_ptr = c->data.ptr;
    }
    strncpy(base_ptr, src_ptr, len);
    c->len = len;
    base_ptr[c->len] = '\0';
    return 0;
  }
  return -1;
}

static int stl_string_create_fmt(stl_string *c, const char *fmt, va_list va)
{
  int rc;
  char tmp[STL_STRING_TEMP_BUFFER_SIZE];
  va_list args;

  va_copy(args, va);
  rc = vsnprintf(tmp, sizeof(tmp), fmt, args);
  if (rc < 0)
  {
    va_end(args);
    return -1;
  }
  va_end(args);
  char *ptr = stl_string_data(c);
  if (rc < STL_STRING_INLINE_LEN)
  {
    memmove(ptr, &tmp, rc);
  }
  else
  {
    c->data.ptr = calloc(rc + 1, sizeof(char));
    assert(c->data.ptr != NULL);
  }
  return 0;
}

int stl_string_sprintf(stl_string *c, const char *fmt, ...)
{
  char *str;
  va_list args;
  va_start(args, fmt);
  int ret = stl_string_create_fmt(c, fmt, args);
  va_end(args);
  return ret;
}
char *stl_string_strchr(stl_string *c, char ch)
{
  if (c != NULL)
  {
    return strchr(stl_string_data(c), ch);
  }
  return NULL;
}

int stl_stringo_xxx(stl_string *c, bool flag)
{
  char *ptr = stl_string_data(c);
  if (ptr != NULL)
  {
    int delta = (flag) ? -32 : 32;
    for (size_t i = 0; i < c->len; i++)
    {
      if ((ptr[i] >= 'A' && ptr[i] <= 'Z' && !flag) || (ptr[i] >= 'a' && ptr[i] <= 'z' && flag))
      {
        ptr[i] += delta;
      }
    }
    return 0;
  }
  return -1;
}
int stl_stringolower(stl_string *c)
{
  return stl_stringo_xxx(c, false);
}
int stl_stringoupper(stl_string *c)
{
  return stl_stringo_xxx(c, true);
}
char *stl_string_strstr(stl_string *c, char *str)
{
  if (c != NULL && str != NULL)
  {
    return strstr(stl_string_data(c), str);
  }
  return NULL;
}
void stl_string_deinit(stl_string *c)
{
  if (c != NULL)
  {
    size_t len = stl_string_len(c);
    if (len >= STL_STRING_INLINE_LEN)
    {
      free(c->data.ptr);
    }
    c->len = 0;
  }
}
void stl_string_destroy(stl_string *c)
{
  if (c != NULL)
  {
    stl_string_deinit(c);
    free(c);
    c = NULL;
  }
}