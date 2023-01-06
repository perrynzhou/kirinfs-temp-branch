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
#define MGNT_PARSE_VAR_LEN 4096
void mgmt_usage()
{
  printf("Usage: mgmt -a {mgmt_addr}  -d {mgmt_data_path} -f {fsname}\n");
  printf("       mgmt -a 127.0.0.1:4567  -d ~/temp  -f bigfs\n");
}

int main(int argc, char *argv[])
{
  stl_string *mgmt_addr_info = NULL;
  stl_string *mgmt_fs_name = NULL;
  stl_string *mgmt_data_dir = NULL;
  int option = 0;
  size_t len = 0;
  char tmp_var[MGNT_PARSE_VAR_LEN] = {'\0'};
  while ((option = getopt(argc, argv, "a:f:d:")) != -1)
  {
    len = strlen(optarg);
    strncpy((char *)&tmp_var, optarg, len);
    switch (option)
    {
    case 'a':
      mgmt_addr_info = stl_string_alloc((char *)&tmp_var);
      break;
    case 'f':
      mgmt_fs_name = stl_string_alloc((char *)&tmp_var);
      break;
    case 'd':
      mgmt_data_dir = stl_string_alloc((char *)&tmp_var);
      break;
    default:
      break;
    }
  }
  // mgmt_service *mgmt_service_alloc(int port, stl_string *fsname, stl_string *dir)

  if (mgmt_addr_info == NULL || mgmt_fs_name == NULL || mgmt_data_dir == NULL)
  {
    mgmt_usage();
    exit(EXIT_FAILURE);
  }

  mgmt_service *service = mgmt_service_alloc(mgmt_addr_info, mgmt_fs_name, mgmt_data_dir);
  mgmt_service_start(service);
  if (!service)
  {
    stl_string_destroy(mgmt_addr_info);
    stl_string_destroy(mgmt_fs_name);
    stl_string_destroy(mgmt_data_dir);
  }

  return 0;
}