/*************************************************************************
    > File Name: stl_utils.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Mon 21 Mar 2022 11:50:59 AM UTC
 ************************************************************************/

#ifndef _STL_UTILS_H
#define _STL_UTILS_H
#define stl_be32_to_cpu(x) ((uint32_t)((((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) | \
                                     (((uint32_t)(x) & (uint32_t)0x0000ff00UL) << 8) |  \
                                     (((uint32_t)(x) & (uint32_t)0x00ff0000UL) >> 8) |  \
                                     (((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24)))

#define stl_be64_to_cpu(x) ((uint64_t)((((uint64_t)(x) & (uint64_t)0x00000000000000ffULL) << 56) | \
                                     (((uint64_t)(x) & (uint64_t)0x000000000000ff00ULL) << 40) | \
                                     (((uint64_t)(x) & (uint64_t)0x0000000000ff0000ULL) << 24) | \
                                     (((uint64_t)(x) & (uint64_t)0x00000000ff000000ULL) << 8) |  \
                                     (((uint64_t)(x) & (uint64_t)0x000000ff00000000ULL) >> 8) |  \
                                     (((uint64_t)(x) & (uint64_t)0x0000ff0000000000ULL) >> 24) | \
                                     (((uint64_t)(x) & (uint64_t)0x00ff000000000000ULL) >> 40) | \
                                     (((uint64_t)(x) & (uint64_t)0xff00000000000000ULL) >> 56)))
#endif
