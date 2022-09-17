/*************************************************************************
    > File Name: test_queue.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 28 Mar 2022 09:44:05 AM UTC
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "../src/inc/c_queue.h"
int queue_traverse(void *data)
{
  int *value = (int *)data;
  // fprintf(stdout, "data=%s,addr=%p\n", value, value);
  return *value;
}

int main(int argc, char *argv[])
{
  size_t n = atoi(argv[1]);
  int *arr = calloc(n, sizeof(int));
  srand(n);
  for (size_t i = 0; i < n; i++)
  {
    arr[i] = rand();
  }
  c_queue_t queue;
  size_t push_op = 0, pop_op = 0, traverse_op = 0;
  c_queue_init(&queue, sizeof(int *), free);
  struct timeval start;
  struct timeval end;
  unsigned long timer;

  gettimeofday(&start, NULL);
  int ret = -1;
  int *data = NULL;
  size_t j = 0;
  while (true)
  {
    if (j > n)
    {
      break;
    }
    for (size_t i = 0; i < n; i++)
    {
      int op = i % 3;
      switch (op)
      {
      case 0:
        ret = c_queue_push(&queue, &arr[i]);
        push_op += 1;
        break;
      case 1:
        data = (int *)c_queue_pop(&queue);
        free(data);
        pop_op += 1;
        break;
      case 2:
        c_queue_traverse(&queue, (c_stack_traverse_cb)queue_traverse);
        traverse_op += 1;
        break;
      default:
        break;
      }
    }
    j++;
  }
  size_t total = push_op + traverse_op + pop_op;

  gettimeofday(&end, NULL);
  timer = end.tv_sec - start.tv_sec;
  fprintf(stdout,"consume seconds:%ld\n",timer);
  printf("total_op %ld,qps = %ld\n", total,total / timer);
  printf("push_op %ld, qps = %ld\n", push_op,push_op / timer);
  printf("traverse_op %ld, qps = %ld\n",traverse_op, traverse_op / timer);
  printf("pop_op  %ld,qps = %ld\n",pop_op, pop_op / timer);
  /*
  size_t n = 5;
  char *data[5] =  {NULL};
  for(size_t i=0;i<n;i++) {
    char buf[256] = {'\0'};
    snprintf((char *)&buf,256,"queue-%ld",i);
    data[i] = strdup((char *)&buf);
    c_queue_push(&queue,(char *)data[i]);
  }
  c_queue_traverse(&queue,(c_stack_traverse_cb)queue_traverse);
  for(size_t i=0;i<n;i++) {
    char *ptr = (char *)c_queue_pop(&queue);
    fprintf(stdout,"i=%ld,data=%s\n",i,ptr);
  }
  */
  c_queue_deinit(&queue);
  return 0;
}