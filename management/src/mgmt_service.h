/*************************************************************************
    > File Name: mgmt_service.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Thu 29 Sep 2022 02:44:37 AM UTC
 ************************************************************************/

#ifndef _MGMT_SERVICE_H
#define _MGMT_SERVICE_H
#include "../../module/stl/src/stl_dict.h"
#include "../../module/stl/src/stl_string.h"
#include "../../module/stl/src/stl_thread.h"
#include "../../module/stl/src/stl_mutex.h"
#include "../../module/stl/src/stl_epoll.h"
#include <jansson.h>
typedef enum
{
   SERVICE_MGS_TYPE = 0,
   SERVICE_OSS_TYPE,
   SERVICE_MDS_TYPE
} connection_type;

typedef struct
{
   // service type
   connection_type type;
   stl_string addr;
   bool runing_status;
} service_connection_info;
typedef struct
{
   // data storage for msg
   stl_string dir;
   // filesystem name
   stl_string fsname;
   // mgs port
   int port;
   // runtime service_addr_info
   stl_dict *service_info;
   // storage json for service_addr_info 
   json_t *service_topo;
   // keep alive for detach mds/oss 
   stl_thread *detach_works;
   size_t num_detach_works;
   // request handle for each diff service
   stl_thread *service_works;
   size_t num_service_works;
   // protect global topo info
   stl_mutex  lock;
   
   // global epoll
   stl_epoll *ep;
} mgmt_service;

mgmt_service *mgmt_service_alloc(int port,stl_string *fsname, stl_string *dir);
int mgmt_service_join_node(mgmt_service *service,service_connection_info *conn_info);
int mgmt_service_expel_node(mgmt_service *service,service_connection_info *conn_info);
int mgmt_service_start(mgmt_service *service);
void mgmt_service_destroy(mgmt_service *service);
#endif
