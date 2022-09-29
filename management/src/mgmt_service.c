/*************************************************************************
    > File Name: mgmt_service.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Thu 29 Sep 2022 02:44:41 AM UTC
 ************************************************************************/

#include  "mgmt_service.h"
#include <stdio.h>
mgmt_service *mgmt_service_alloc(int port,stl_string *fsname, stl_string *dir)
{
  mgmt_service  *mgmt=NULL;
  stl_epoll *ep = NULL;
   if(fsname && dir) 
   {
     ep = stl_epoll_alloc("127.0.0.1", 1, 1024,NULL);
     
     mgmt = calloc(1,sizeof(mgmt_service));
     mgmt->ep = ep;

   }
out:
   return mgmt;
}
int mgmt_service_join_node(mgmt_service *service,service_connection_info *conn_info)
{
  return 0;
}
int mgmt_service_expel_node(mgmt_service *service,service_connection_info *conn_info)
{
  return 0;
}
int mgmt_service_start(mgmt_service *service)
{
  if(service) {
       stl_epoll_run(service->ep);
  }
}
void mgmt_service_destroy(mgmt_service *service)
{
  if(service) {
    stl_epoll_stop(service->ep);
    stl_epoll_destroy(service->ep);
  }
}