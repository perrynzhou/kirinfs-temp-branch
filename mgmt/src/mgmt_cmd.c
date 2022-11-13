/*************************************************************************
    > File Name: mgmt_cmd.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Wed 05 Oct 2022 06:56:14 AM UTC
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mgmt_cmd.h"
mgmt_request *mgmt_request_alloc(char *service_addr, mgmt_node_type  node_type,mgmt_cmd_type cmd_type)
{
 
  mgmt_request *request = NULL;
  if (service_addr)
  {
    size_t len = strlen(service_addr);
    request = (mgmt_request *)calloc(1,sizeof(mgmt_request)+len);
    strncpy((char *)&request->service_node_addr,service_addr,len);
    request->len = len;
    request->cmd_type = cmd_type;
    request->node_type = node_type;
  }
  return request;
}
void mgmt_request_destroy(mgmt_request *req)
{
  if(req) {
     free(req);
     req= NULL;
  }
}