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
  stl_string addr;
  int port;
  int  service_type;
  bool status;
  int fd;
  void *ctx;
}service_node;

service_node *service_node_create(stl_string *host,int port);
int service_node_keep_alive(service_node *node,void (*ctx_func_cb)(void *ctx));
void service_node_destroy(service_node *node);
#endif
