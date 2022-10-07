/*
 * File            : test_stl_epoll_service.c
 * Author          : ZhangLe
 * CreateTime      : 2022-10-07 03:12:21
 * LastModified    : 2022-10-07 08:24:37
 * Vim             : ts=4, sw=4
 */

#include "../src/stl_epoll.h"
#include "../src/stl_thread.h"
#include <stdio.h>

typedef struct demo_service{
    char* str;
    int port;
} demo_service;

int demo_handle_io(void* ctx, int fd) {
    printf("demo handle io\n");
}

demo_service *demo_service_alloc(int port, char *str) {
    char addr[] = "127.0.0.1:8081";
    demo_service *service = NULL;
    stl_epoll *ep = NULL;
    ep = stl_epoll_alloc(addr, 1, 1024, (stl_epoll_io_func)&demo_handle_io);
    return service;
}

int main() {
    demo_service *service = demo_service_alloc(8081, "hello world");

    //if (service) {
    //    demo_service_start(service);
    //}

    return 0;
}
