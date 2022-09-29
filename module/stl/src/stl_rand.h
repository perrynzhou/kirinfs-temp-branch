/*************************************************************************
    > File Name: stl_rand.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 05 Sep 2022 11:10:42 PM EDT
 ************************************************************************/

#ifndef _STL_RAND_H
#define _STL_RAND_H
#include <stdint.h>
#include <stdio.h>
// update from redis
int32_t stl_rand_int32();
int stl_rand_string(char *buf, size_t n);
int stl_gen_uuid(char *buf,size_t n);
#endif
