/*************************************************************************
    > File Name: service_node.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sun 02 Oct 2022 05:36:19 AM UTC
 ************************************************************************/

#include "service_node.h"
#include "stl_socket.h"
#include "mgmt_cmd.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#define SERVICE_NODE_HANDSHARK_KEY 's'
#define SERVICE_NODE_PING_FAIL_CNT 3

int service_node_init(service_node *sn,stl_string *host,int port,int node_type)
{

  if (sn && host)
  {
    sn->node_addr=stl_string_alloc(stl_string_data(host));
    sn->port = port;
    sn->fd = stl_socket_init_client(stl_string_data(host), port);
    if (sn->fd < 0)
    {
      goto init_err;
    }
    switch(node_type)
    {
      case MGMT_META_NODE:
        sn->node_type_key = stl_string_alloc("meta");
        sn->node_type_val =MGMT_META_NODE;
      break;
      case MGMT_DATA_NODE:
        sn->node_type_key = stl_string_alloc("data");
        sn->node_type_val =MGMT_DATA_NODE;
      break;
      default:
      break;
    }
  
  }
  return 0;

init_err:
  str_string_destroy(sn->node_addr);
  return -1;
}
void service_node_deinit(service_node *sn)
{
  if(sn)
  {
     str_string_destroy(sn->node_addr);
      str_string_destroy(sn->node_type_key);
  }
}
service_node *service_node_alloc(stl_string *host, int port,int node_type)
{
  service_node *sn = (service_node *)alloc(1, sizeof(service_node));
  assert(sn != NULL);

  if (service_node_init(sn, host, port,node_type) != 0)
  {
    free(sn);
    sn = NULL;
  }
  return sn;
}

static long long tm_to_ns(struct timespec tm)
{
  return tm.tv_sec * 1000000000 + tm.tv_nsec;
}

static struct timespec ns_to_tm(long long ns)
{
  struct timespec tm;
  tm.tv_sec = ns / 1000000000;
  tm.tv_nsec = ns - (tm.tv_sec * 1000000000);
  return tm;
}

int service_node_keep_alive(service_node *node, void (*ctx_func_cb)(void *ctx))
{

  size_t fail_cnt = 0;
  struct timespec start_tm;
  struct timespec end_tm;
  int timeout_ms = 2500;

  pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

  clock_gettime(CLOCK_MONOTONIC, &start_tm);
  end_tm = ns_to_tm(tm_to_ns(start_tm) + timeout_ms * 1000000);

  pthread_mutex_lock(&mtx);
  char c = 0;
  while (1)
  {
    if (pthread_cond_timedwait(&cond, &mtx, &end_tm) == ETIMEDOUT)
    {
      if (fail_cnt == SERVICE_NODE_PING_FAIL_CNT)
      {
        break;
      }
      // send message to client
      write(node->fd, SERVICE_NODE_HANDSHARK_KEY, sizeof(char));
      // recv message from server
      read(node->fd, &c, sizeof(char));
    }
    pthread_mutex_unlock(&mtx);
  }
}
void service_node_destroy(service_node *node)
{
  if (node)
  {
    if (node->fd)
    {
      close(node->fd);
    }
    stl_string_deinit(&node->addr);
    free(node);
    node = NULL;
  }
}