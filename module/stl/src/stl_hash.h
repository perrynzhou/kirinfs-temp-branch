/*************************************************************************
    > File Name: stl_hash.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Tue 22 Mar 2022 08:41:36 AM UTC
 ************************************************************************/

#ifndef __STL_HASH_H
#define __STL_HASH_H
#include <stdint.h>
#include <stdio.h>
uint32_t stl_hash_crc16(const char* key, size_t key_length);
uint32_t stl_hash_crc32(const char *key, size_t key_length);
uint32_t stl_hash_crc32a(const char *key, size_t key_length);
uint32_t stl_hash_fnv1_64(const char *key, size_t key_length);
uint32_t stl_hash_hsieh(const char *key, size_t key_length);
uint32_t stl_hash_jenkins(const char *key, size_t length);
uint32_t stl_hash_fnv1a_64(const char *key, size_t key_length);
uint32_t stl_hash_murmur(const char *key, size_t length);
uint32_t stl_hash_fnv1_32(const char *key, size_t key_length);
uint32_t stl_hash_fnv1a_32(const char *key, size_t key_length);
uint32_t stl_hash_one_at_a_time(const char *key, size_t key_length);
uint32_t stl_hash_jump_consistent(uint64_t key, int32_t num_buckets);
uint64_t stl_hash_gfs(const char *msg, int len);
uint32_t stl_hash_super_fast(const char *data, int32_t len);
#endif
