/*************************************************************************
  > File Name: c_string.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Tue 22 Mar 2022 07:36:46 AM UTC
 ************************************************************************/

#ifndef __STL_STRING_H
#define __STL_STRING_H
#include <stdbool.h>
#include <stdio.h>
#define STL_STRING_INLINE_LEN   (8)
// if string len <8 ,inline data in string
typedef struct
{
  union
  {
    char *ptr;
    char in_data[STL_STRING_INLINE_LEN];
  } data;
  size_t len;
} stl_string;

stl_string *stl_string_alloc(char *data);
void   stl_string_reset(stl_string *c);
int stl_string_init(stl_string *c, char *data);
stl_string *stl_string_empty(size_t size);
int stl_string_dup(stl_string *src,stl_string *dst);
void stl_string_trim(stl_string *src);
int stl_string_replace(stl_string *src,const char *old_data,const char *new_data);
int stl_string_split(stl_string *src,const char *delim,stl_string **c_ptr,size_t *cnt);
int stl_string_sprintf(stl_string *c, const char *fmt, ...);
bool stl_string_hassuffix(stl_string *c, char *s);
bool stl_string_hasprefix(stl_string *c, char *s);
int stl_string_append(stl_string *c, char *str);
char *stl_string_strchr(stl_string *c, char ch);
char *stl_string_strstr(stl_string *c, char *str);
int stl_string_tolower(stl_string *c);
int stl_string_tooupper(stl_string *c);

size_t stl_string_len(stl_string *c);
char *stl_string_data(stl_string *c);
void stl_string_deinit(stl_string *c);
void stl_string_destroy(stl_string *c);
#endif
