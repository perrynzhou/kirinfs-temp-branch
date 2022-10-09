/*
 * File            : test_stl_epoll_service.c
 * Author          : ZhangLe
 * CreateTime      : 2022-10-07 03:12:21
 * LastModified    : 2022-10-09 18:38:39
 * Vim             : ts=4, sw=4
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/stl_epoll.h"
#include "../src/stl_thread.h"
#include "./test_stl_epoll.h"


int demo_handle_io(void* ctx, int fd) {
    printf("demo handle io\n");
}

int main() {
    char addr[] = "127.0.0.1:8081";
    demo_service *demo = NULL;
    stl_epoll *ep = NULL;

    ep = stl_epoll_alloc(addr, 1, 1024, (stl_epoll_io_func)&demo_handle_io);
    demo = calloc(1, sizeof(demo_service));
    demo->ep = ep;
    demo->ep->io_func_ctx = demo;

    stl_epoll_run(demo->ep);
    return 0;
}
