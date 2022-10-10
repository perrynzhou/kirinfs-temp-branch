/*
 * File            : test_stl_epoll_service.c
 * Author          : ZhangLe
 * CreateTime      : 2022-10-07 03:12:21
 * LastModified    : 2022-10-10 12:00:16
 * Vim             : ts=4, sw=4
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../src/stl_epoll.h"
#include "../src/stl_thread.h"
#include "./test_stl_epoll.h"

#define SERVICE_MAX_DBUF_SIZE (4096)

typedef struct demo_service{
    char* str;
    int num;
    stl_epoll *ep;
} demo_service;

int demo_handle_io(void* ctx, int fd) {
    printf("demo handle io\n");
    int len = -1;
    int num = -1;
    char dbuf[4096] = {'\0'};

    do{
        len = read(fd, (char *)&dbuf, SERVICE_MAX_DBUF_SIZE);
        num = ((demo_request *)&dbuf)->num;
        //printf("num = %d\n", num);
    } while(len > 0);

    if (len == 0) {
        num = ((demo_request *)&dbuf)->num;
        printf("len = 0, num = %d\n", num);
    }

    return 0;
}

int main() {
    char addr[] = "127.0.0.1:8081";
    demo_service *demo = NULL;
    stl_epoll *ep = NULL;

    ep = stl_epoll_alloc(addr, 1, 1024, (stl_epoll_io_func)&demo_handle_io);
    demo = calloc(1, sizeof(demo_service));
    demo->ep = ep;
    demo->ep->io_func_ctx = demo;

    if (demo) {
        printf("demo\n");
        stl_epoll_run(demo->ep);
    }
    return 0;
}
