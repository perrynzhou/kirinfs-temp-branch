/*************************************************************************
    > File Name: stl_sha1.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Mon 12 Sep 2022 03:54:36 AM EDT
 ************************************************************************/

#ifndef _STL_SHA1_H
#define _STL_SHA1_H
#include <stdint.h>
// abstract form nginx
typedef struct {
    uint64_t  bytes;
    uint32_t  a, b, c, d, e, f;
    unsigned char    buffer[64];
} stl_sha1;

stl_sha1 *stl_sha1_alloc();
int stl_sha1_init(stl_sha1 *ctx);
void stl_sha1_update(stl_sha1 *ctx, const void *data, size_t size);
void stl_sha1_final(unsigned char result[20], stl_sha1 *ctx);
int  stl_sha1_deinit(stl_sha1 *ctx);
void  stl_sha1_destroyt(stl_sha1 *ctx);
#endif
