/*************************************************************************
    > File Name: stl_crc.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Tue 22 Mar 2022 08:45:19 AM UTC
 ************************************************************************/

#ifndef _STL_CRC_H
#define _STL_CRC_H
#include <stdint.h>
uint64_t stl_crc64(const char *s, int l);
uint16_t stl_crc16(const char *buf, int len);
uint32_t stl_crc32(const char* s, int len);
#endif
