/*************************************************************************
    > File Name: mgmt_service.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Thu 29 Sep 2022 02:44:41 AM UTC
 ************************************************************************/

#include "mgmt_service.h"
#include "../../module/stl/src/stl_hash.h"
#include "service_node.h"
#include "mgmt_cmd.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define SERVICE_MAX_NODE_COUNT (4096)
#define SERVICE_MAX_DBUF_SIZE (4096)

int mgmt_handle_io(void *ctx, int fd)
{
  int ret = -1;
  char dbuf[4096] = {'\0'};
  ssize_t rn = -1;
  stl_string **node_info_ptr = NULL;
  size_t cnt = 0;
  do
  {
    rn = read(fd, (char *)&dbuf, SERVICE_MAX_DBUF_SIZE);
  } while (rn > 0);
  if (rn > 0)
  {

    size_t mgmt_request_head_len = ((mgmt_request *)&dbuf)->len;
    mgmt_cmd_type cmd_type = ((mgmt_request *)&dbuf)->cmd_type;
    mgmt_node_type node_type =  ((mgmt_request *)&dbuf)->node_type;
    char *service_node_addr = ((mgmt_request *)&dbuf)->service_node_addr;
    stl_string *node_addr = stl_string_alloc(service_node_addr);
    mgmt_service *service = (mgmt_service *)ctx;

    stl_string_split(node_addr, ":", &node_info_ptr, &cnt);
    int node_port = atoi(stl_string_data(node_info_ptr[1]));
    service_node sci;
    ret = service_node_init(&sci, node_info_ptr[0], node_port,node_type);
    if (ret < 0)
    {
      switch (cmd_type)
      {
      case MGMT_DEL_NODE_CMD:
        ret = mgmt_service_join_node(service, &sci);
         
        break;
      case MGMT_ADD_NODE_CMD:
        ret = mgmt_service_expel_node(service, &sci);
        break;
      default:
        break;
      }
    }
    service_node_deinit(&sci);
    stl_string_destroy(node_addr);
  }
  if (ret != 0)
  {
    for (size_t i = 0; i < cnt; i++)
    {
      stl_string_destroy(node_info_ptr[i]);
    }
  }
  if (node_info_ptr)
  {
    free(node_info_ptr);
    node_info_ptr = NULL;
  }
  return ret;
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
mgmt_service *mgmt_service_alloc(stl_string  *addr, stl_string *fsname, stl_string *dir)
{
  mgmt_service *mgmt = NULL;
  stl_epoll *ep = NULL;
  if (fsname && dir)
  {
    char *addr_info = stl_string_data(addr);
    ep = stl_epoll_alloc(addr_info, 1, 1024, (stl_epoll_io_func)&mgmt_handle_io);
    mgmt = calloc(1, sizeof(mgmt_service));
    mgmt->ep = ep;
    mgmt->service_info = stl_dict_alloc(SERVICE_MAX_NODE_COUNT, mgmt_service_node_cmp, stl_hash_crc32a);
    mgmt->detach_works = (stl_thread **)calloc(SERVICE_MAX_NODE_COUNT, sizeof(stl_thread *));
    mgmt->num_detach_works = 0;
    mgmt->ep->io_func_ctx = mgmt;

   stl_string_init(&mgmt->fsname,stl_string_data(fsname));
   stl_string_init(&mgmt->dir,stl_string_data(dir));

  }
out:
  return mgmt;
}
int mgmt_service_join_node(mgmt_service *service, service_node *node_info)
{
  int ret = -1;
  stl_string **data = NULL;
  if (service && node_info)
  {
    char *info_str = stl_string_data(&node_info->node_addr);
    stl_dict_item key = {
        .data.str = info_str,
        .data_type = DICT_ITEM_TYPE_STR,
        .len = strlen(info_str),
    };
    if (!stl_dict_lookup(service->service_info, &key))
    {

      size_t cnt = 0;
      stl_string_split(&node_info->node_addr, ":", &data, &cnt);
      if (cnt == 2)
      {
        
        
        service_node *node = service_node_alloc(node_info->node_addr,node_info->port,node_info->node_type_val);
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
  if (data)
  {
    free(data);
    data = NULL;
  }
  return ret;
}
int mgmt_service_expel_node(mgmt_service *service, service_node *node_info)
{
  int ret = -1;
  if (service && node_info)
  {
    stl_dict_item key = {
        .data.str = stl_string_data(node_info->node_addr),
        .data_type = DICT_ITEM_TYPE_STR,
        .len = strlen(stl_string_data(node_info->node_addr)),
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