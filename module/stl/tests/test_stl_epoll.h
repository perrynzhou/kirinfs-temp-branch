/*
 * File            : test_stl_epoll.h
 * Author          : ZhangLe
 * CreateTime      : 2022-10-08 18:49:27
 * LastModified    : 2022-10-09 18:47:03
 * Vim             : ts=4, sw=4
 */

#ifndef _TEST_STL_EPOLL_H_
#define _TEST_STL_EPOLL_H_

#include "../src/stl_epoll.h"

typedef struct demo_service{
    char* str;
    int num;
    stl_epoll *ep;
} demo_service;

typedef struct demo_request{
    size_t len;
    int num;
} demo_request;

#endif    /* test_stl_epoll.h */

