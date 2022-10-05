/*************************************************************************
    > File Name: test_stack.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Mon 28 Mar 2022 06:41:25 AM UTC
 ************************************************************************/

#include <stdio.h>
#include "../src/inc/c_stack.h"
int stack_traverse(void *data)
{
  char *value = (char *)data;
  fprintf(stdout, "data=%s,addr=%p\n", value, value);
  return 0;
}

int main()
{
  c_stack_t stack;
  c_stack_init(&stack,sizeof(void *),8,false,(c_stack_free_cb)&free);
  size_t n = 5;
  char *data[5] =  {NULL};
  for(size_t i=0;i<n;i++) {
    char buf[256] = {'\0'};
    snprintf((char *)&buf,256,"stack-%ld",i);
    data[i] = strdup((char *)&buf);
    c_stack_push(&stack,(char *)data[i]);
  }
  c_stack_traverse(&stack,(c_stack_traverse_cb)stack_traverse);
  for(size_t i=0;i<n;i++) {
    char *ptr = (char *)c_stack_pop(&stack);
    fprintf(stdout,"i=%ld,data=%s\n",i,ptr);
  }
  c_stack_deinit(&stack);
  return 0;
}