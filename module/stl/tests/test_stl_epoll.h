/*
 * File            : test_stl_epoll.h
 * Author          : ZhangLe
 * CreateTime      : 2022-10-08 18:49:27
 * LastModified    : 2022-10-09 08:23:49
 * Vim             : ts=4, sw=4
 */

#ifndef _TEST_STL_EPOLL_H_
#define _TEST_STL_EPOLL_H_

#include "../src/stl_epoll.h"

typedef struct demo_request{
    size_t len;
    int num;
} demo_request;

#endif    /* test_stl_epoll.h */
