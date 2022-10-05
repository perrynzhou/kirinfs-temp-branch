/*************************************************************************
  > File Name: mgmt_cmd.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Wed 05 Oct 2022 06:56:10 AM UTC
 ************************************************************************/

#ifndef _MGMT_CMD_H
#define _MGMT_CMD_H
#include <stdio.h>
typedef enum {
   MGMT_DEL_NODE_CMD=0,
   MGMT_ADD_NODE_CMD,
   MGMT_MNT_CMD,
}mgmt_cmd_type;

typedef enum {
   MGMT_META_NODE=0,
   MGMT_DATA_NODE,
}mgmt_node_type;
typedef struct  {
   size_t  len;
   mgmt_cmd_type cmd_type;
   mgmt_node_type  node_type;
  char service_node_addr[0];
}mgmt_request;

mgmt_request *mgmt_request_alloc(char *service_addr, mgmt_node_type  node_type,mgmt_cmd_type cmd_type);
void mgmt_request_destroy(mgmt_request *req);
#endif
