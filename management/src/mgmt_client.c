/*************************************************************************
  > File Name: mgmt_client.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Thu 29 Sep 2022 06:43:19 AM UTC
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "mgmt_cmd.h"
#include "../../module/stl/src/stl_socket.h"
#include "../../module/stl/src/stl_string.h"
void mgmt_client_usage()
{
    printf("Usage:  mgmt_client -s {mgmt_addr} -n {service_addr} -t {service_cmd_type}\n");
    printf("        mgmt_client -s 10.211.55.5  -n 127.0.0.3  -t  0\n");
    printf("        mgmt_client -s 10.211.55.5  -n 127.0.0.1  -t  1\n");
}

static void mgmt_request_print(mgmt_request *req, char *mgmt_addr)
{
    if (req)
    {
        switch (req->type)
        {
        case MGMT_DEL_NODE_CMD:
            fprintf(stdout, "mgmt-%s del %s success\n", mgmt_addr, (char *)&req->service_node_addr);
            break;
        case MGMT_ADD_NODE_CMD:
            fprintf(stdout, "mgmt-%s add %s success\n", mgmt_addr, (char *)&req->service_node_addr);
            break;
        default:
            break;
        }
    }
}
int main(int argc, char *argv[])
{
    int option = 0;
    char *mgmt_addr = NULL;
    char *service_addr = NULL;
    int cmd_type = -1;
    while ((option = getopt(argc, argv, "s:n:t:")) != -1)
    {
        switch (option)
        {
        case 's':
            mgmt_addr = strdup(optarg);
            break;
        case 'n':
            service_addr = strdup(optarg);
            break;
        case 't':
            cmd_type = atoi(optarg);
            break;
        default:
            mgmt_client_usage();
            exit(EXIT_FAILURE);
        }
    }
    if (service_addr == NULL || mgmt_addr == NULL || cmd_type == -1)
    {
        mgmt_usage();
        exit(EXIT_FAILURE);
    }
    mgmt_request *req = mgmt_request_alloc(service_addr, cmd_type);
    if (req)
    {
        stl_string *mgmt_addr_tmp = stl_string_alloc(mgmt_addr);
        stl_string **ptr = NULL;
        size_t cnt = 0;
        int ret = stl_string_split(mgmt_addr_tmp, ":", &ptr, &cnt);
        if (!ret)
        {
            char *host = stl_string_data(ptr[0]);
            int port = atoi(stl_string_data(ptr[1]));
            int fd = stl_socket_init_client(host, port);
            int try = 0, max_try = 3;
            while (try < max_try)
            {
                ret = write(fd, req, sizeof(*req) + strlen(service_addr) + 1);
                if (ret > 0)
                {
                    mgmt_request_print(req, stl_string_data(mgmt_addr_tmp));
                    break;
                }
                try++;
            }
        }

        for(size_t i=0;i<cnt;i++)
        {
             stl_string_destroy(ptr[i]);
        }
        if(ptr !=NULL)
        {
            free(ptr);
        }
        if(mgmt_addr_tmp)
        {
            stl_string_destroy(mgmt_addr_tmp);
        }
    }
    return 0;
}