/*************************************************************************
    > File Name: service_node.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sun 02 Oct 2022 05:36:15 AM UTC
 ************************************************************************/

#ifndef _SERVICE_NODE_H
#define _SERVICE_NODE_H
#include "stl_string.h"
typedef struct  {
  stl_string *node_addr;
  // meta or data
  stl_string  *node_type_key;
  mgmt_node_type  node_type_val;
  bool status;
  int port;
  int fd;
  void *ctx;
}service_node;


int service_node_init(service_node *sn,stl_string *host,int port,int node_type);
service_node *service_node_alloc(stl_string *host,int port,int node_type);
int service_node_keep_alive(service_node *node,void (*ctx_func_cb)(void *ctx));
void service_node_destroy(service_node *node);
void service_node_deinit(service_node *sn);
#endif
