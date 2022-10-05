/*************************************************************************
    > File Name: mgmt_service.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Thu 29 Sep 2022 02:44:41 AM UTC
 ************************************************************************/

#include "mgmt_service.h"
#include "stl_hash.h"
#include "service_node.h"
#include <stdio.h>
#define SERVICE_MAX_NODE_COUNT (4096)


int mgmt_handle_io(void *ctx, int fd)
{
  return 0;
}


int mgmt_service_node_cmp(stl_dict_item *a, stl_dict_item *b)
{
  if (a && b)
  {
    return strncmp(a->data.str, b->data.str, strlen(a->data.str));
  }
  return -1;
}
static int mgmt_service_modify_topo(mgmt_service *service)
{
  return 0;
}
mgmt_service *mgmt_service_alloc(int port, stl_string *fsname, stl_string *dir)
{
  mgmt_service *mgmt = NULL;
  stl_epoll *ep = NULL;
  if (fsname && dir)
  {
    ep = stl_epoll_alloc("127.0.0.1", 1, 1024, (stl_epoll_io_func)&mgmt_handle_io);
    mgmt = calloc(1, sizeof(mgmt_service));
    mgmt->ep = ep;
    mgmt->service_info = stl_dict_alloc(SERVICE_MAX_NODE_COUNT, mgmt_service_node_cmp, stl_hash_crc32a);
    mgmt->detach_works = (stl_thread **)calloc(SERVICE_MAX_NODE_COUNT, sizeof(stl_thread *));
    mgmt->num_detach_works = 0;
  }
out:
  return mgmt;
}
int mgmt_service_join_node(mgmt_service *service, service_connection_info *conn_info)
{
  int ret = -1;
  if (service && conn_info)
  {
    char *info_str = stl_string_data(&conn_info->addr);
    stl_dict_item key = {
        .data.str = info_str,
        .data_type = DICT_ITEM_TYPE_STR,
        .len = strlen(info_str),
    };
    if (!stl_dict_lookup(service->service_info, &key))
    {
      stl_string **data = NULL;
      size_t cnt = 0;
      stl_string_split(&conn_info->addr, ":", data, &cnt);
      if (cnt == 2)
      {

        service_node *node = service_node_create(data[0], atoi(stl_string_data(data[1])));
        assert(node != NULL);
        stl_dict_item value = {
            .data.ptr = node,
            .data_type = DICT_ITEM_TYPE_PTR,
            .len = sizeof(*node),
        };
        stl_dict_insert(service->service_info, &key, &value);
        node->ctx = service;
        service_node_keep_alive(node, mgmt_service_modify_topo);
        service->detach_works++;
      }
    }
  }
out:
  return ret;
}
int mgmt_service_expel_node(mgmt_service *service, service_connection_info *conn_info)
{
  int ret = -1;
  if (service && conn_info)
  {
    stl_dict_item key = {
        .data.str = stl_string_data(&conn_info->addr),
        .data_type = DICT_ITEM_TYPE_STR,
        .len = strlen(stl_string_data(&conn_info->addr)),
    };
    void *ptr = NULL;
    if ((ptr = stl_dict_lookup(service->service_info, &key)))
    {
      service_node *node = (service_node *)ptr;
      stl_dict_delete(service->service_info, &key);
    }
  }
  return ret;
}
int mgmt_service_start(mgmt_service *service)
{
  if (service)
  {
    stl_epoll_run(service->ep);
  }
}
void mgmt_service_destroy(mgmt_service *service)
{
  if (service)
  {
    stl_epoll_stop(service->ep);
    stl_epoll_destroy(service->ep);
  }
}