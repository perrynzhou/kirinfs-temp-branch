/*
 * File            : test_stl_epoll_service.c
 * Author          : ZhangLe
 * CreateTime      : 2022-10-07 03:12:21
 * LastModified    : 2022-10-09 08:31:25
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
    ssize_t rn = -1;
    char dbuf[4096] = {'\0'};

    do{
        rn = read(fd, (char *)&dbuf, SERVICE_MAX_DBUF_SIZE);
        int num = ((demo_request *)&dbuf)->num;
        printf("num %d\n", num);
        printf("fd %d\n", fd);
    } while(rn > 0);

    if (rn > 0) {
        printf("demo handle io\n");
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
        stl_epoll_run(demo->ep);
    }
    return 0;
}
