/*
 * File            : test_stl_epoll_service.c
 * Author          : ZhangLe
 * CreateTime      : 2022-10-07 03:12:21
 * LastModified    : 2022-10-08 19:24:38
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
    demo_service *service = NULL;
    stl_epoll *ep = NULL;

    ep = stl_epoll_alloc(addr, 1, 1024, (stl_epoll_io_func)&demo_handle_io);

    service = calloc(1, sizeof(demo_service));
    service->ep = ep;

    stl_epoll_run(service->ep);
    return 0;
}
