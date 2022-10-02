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
void print_usage()
{
  printf("Usage: mgmt  -p {port}\n");
}

int main(int argc, char *argv[])
{
  int option = 0;
  int port = -1;
  while ((option = getopt(argc, argv, "p:")) != -1)
  {
    switch (option)
    {
    case 'p':
      port = atoi(optarg);
      break;
    default:
      break;
    }
  }
  if (port == -1)
  {
    print_usage();
    exit(EXIT_FAILURE);
  }
  
  return 0;
}