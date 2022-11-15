/*************************************************************************
  > File Name: mgmt_main.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Thu 29 Sep 2022 06:13:54 AM UTC
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "mgmt_service.h"
#include "../../module/stl/src/stl_socket.h"
#include "../../module/stl/src/stl_string.h"
void mgmt_usage()
{
  printf("Usage: mgmt -p {mgmt_port}  -d {mgmt_data_path} -f {fsname}\n");
  printf("       mgmt -p 4567  -d /data  -f bigfs\n");
}

int main(int argc, char *argv[])
{
  int mgmt_run_port = -1;
  char *fsname_temp = NULL;
  char *mgmt_data_temp = NULL;
  int option = 0;
  while ((option = getopt(argc, argv, "p:f:d:")) != -1)
  {
    switch (option)
    {
    case 'p':
      mgmt_run_port = atoi(optarg);
      break;
    case 'f':
      fsname_temp = strdup(optarg);
      break;
    case 'd':
      mgmt_data_temp = strdup(optarg);
      break;
    default:
      break;
    }
  }
  // mgmt_service *mgmt_service_alloc(int port, stl_string *fsname, stl_string *dir)

  if (mgmt_run_port == -1 || fsname_temp == NULL || mgmt_data_temp == NULL)
  {
    mgmt_usage();
    exit(EXIT_FAILURE);
  }
  stl_string **ptr = NULL;
  stl_string *fsname = stl_string_alloc(fsname_temp);
  stl_string *mgmt_data = stl_string_alloc(mgmt_data_temp);

  mgmt_service *service = mgmt_service_alloc(mgmt_run_port, fsname, mgmt_data);
  if (service)
  {
    free(fsname_temp);
    free(mgmt_data_temp);
    stl_string_destroy(fsname);
    stl_string_destroy(mgmt_data);
    mgmt_service_start(service);
  }

  return 0;
}