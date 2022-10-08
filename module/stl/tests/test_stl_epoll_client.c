/*
 * File            : test_stl_epoll_client.c
 * Author          : ZhangLe
 * CreateTime      : 2022-10-08 18:44:34
 * LastModified    : 2022-10-08 19:24:36
 * Vim             : ts=4, sw=4
 */

#include <stdio.h>
#include <stdlib.h>
#include "./test_stl_epoll.h"

demo_request* demo_request_alloc(char* service_addr, char* str) {

    demo_request* request = NULL;

    if (service_addr) {
        size_t len = strlen(service_addr);
        request = (demo_request *)calloc(1, sizeof(demo_request) + len);
        request->num = 2;
    }

    return request;
}

static void demo_request_print(demo_request* req) {
    printf("hello new client %s", (char *)&req->service_node_addr);
}

int main(int argc, char *argv[]){
    int fd = 0;
    int ret = -1;
    char service_addr[] = "127.0.0.1:8081";
    demo_request *req = demo_request_alloc(service_addr);

    if (req) {
        fd = stl_socket_init_client("127.0.0.1", "8082");
        ret = write(fd, req, size(*req) + str(service_addr) + 1);
        demo_request_print(req);
    }

    return 0;
}
