/*************************************************************************
  > File Name: stl_md5.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Thu 04 Aug 2022 10:50:07 AM CST
 ************************************************************************/

#ifndef _STL_MD5_H
#define _STL_MD5_H
#include <stdint.h>
#include <stdio.h>
typedef unsigned char uchar;
typedef struct {
    uint64_t  bytes;
    uint32_t  a, b, c, d;
    uchar    buffer[64];
} stl_md5;

stl_md5 *stl_md5_alloc();
void stl_md5_init(stl_md5 *ctx);
void stl_md5_compute(stl_md5 *ctx, const void *data, size_t size);
void stl_md5_checksum(stl_md5 *ctx,char result[16]);
void stl_md5_destroy(stl_md5 *ctx);
#endif