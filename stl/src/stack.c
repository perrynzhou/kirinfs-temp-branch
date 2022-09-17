/*************************************************************************
    > File Name: stack.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 28 Mar 2022 05:37:42 AM UTC
 ************************************************************************/

#include "stack.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#define STACK_INCR_FACCTOR (0.25)
#define STACK_INIT_SIZE (8)
stack *stack_alloc(size_t max_count, bool auto_expand)
{
  stack *sk = (stack *)calloc(1, sizeof(stack));
  assert(sk != NULL);
  if (stack_init(sk, max_count, auto_expand) != 0)
  {
    free(sk);
    sk = NULL;
  }
  return sk;
}
static inline int stack_expand(stack *sk)
{
  int ret = -1;
  if (sk)
  {
    size_t max_count = sk->max_count * (1 + STACK_INCR_FACCTOR);
    sk->ptr = (void **)realloc(sk->ptr, sizeof(void *) * max_count);
    sk->max_count = max_count;
    ret = 0;
  }
  return ret;
}
int stack_init(stack *sk, size_t max_count, bool auto_expand)
{
  if (sk != NULL && max_count > 0)
  {
    max_count = (max_count <= STACK_INIT_SIZE) ? STACK_INIT_SIZE : max_count;
    sk->ptr = (void **)calloc(max_count, sizeof(void *));
    assert(sk->ptr != NULL);
    sk->cur_count =  ATOMIC_VAR_INIT(0);
    sk->max_count = max_count;
    sk->auto_expand = auto_expand;
    return 0;
  }
  return -1;
}
int stack_push(stack *sk, void *data)
{
  if (sk != NULL && data != NULL)
  {
    if (sk->cur_count >= sk->max_count && sk->auto_expand && stack_expand(sk) != 0)
    {
      return -1;
    }
    if (sk->cur_count < sk->max_count)
    {
      sk->ptr[sk->cur_count] = data;
      atomic_fetch_add(&sk->cur_count,1);
      return 0;
    }
  }
  return -1;
}
void *stack_pop(stack *sk)
{
  void *ptr = NULL;
  if (sk != NULL && sk->cur_count > 0)
  {
    atomic_fetch_sub(&sk->cur_count,1);
    ptr = sk->ptr[sk->cur_count];
  }
  return ptr;
}
void stack_deinit(stack *sk)
{
  if (sk != NULL && sk->ptr != NULL)
  {
    free(sk->ptr);
    sk->ptr = NULL;
  }
}
void stack_destroy(stack *sk)
{
  if (sk != NULL)
  {
    stack_deinit(sk);
    free(sk);
    sk = NULL;
  }
}
int stack_traverse(stack *sk, stack_traverse_cb traverse_cb)
{
  if (sk != NULL && sk->ptr != NULL)
  {
    for (size_t i = 0; i < sk->cur_count; i++)
    {
      if (traverse_cb != NULL)
      {
        traverse_cb(sk->ptr[i]);
      }
    }
  }
  return 0;
}
#ifdef TEST
#include <threads.h>
#include <stdatomic.h>
int test_stack_traverse_cb(void *arg) {
  int *v = (int *)arg;
  fprintf(stdout,"traverse:data=%d\n",*v);
  return 0;
}
void *sum(void *arg)
{
  int *v = (int *)arg;
  for (int i = 0; i < 100; i++)
  {
     atomic_fetch_add(v, 1);
  }
  fprintf(stdout,"v=%d\n",*v);
  return NULL;
}
#include <stdlib.h>
int main(int argc, char *argv[])
{
  int n = atoi(argv[1]);
  stack sk;
  stack_init(&sk, 32, true);
  int *arr = (int *)calloc(n, sizeof(int));
  for (int i = 0; i < n; i++)
  {
    arr[i] = i + 1;
    int ret = stack_push(&sk, &arr[i]);
    fprintf(stdout, "push::data=%d,stack len=%d\n", arr[i], stack_len(&sk));
  }
    stack_traverse(&sk,test_stack_traverse_cb);

  for (int i = 0; i < n; i++)
  {
    int *v = stack_pop(&sk);
    fprintf(stdout, "pop::data=%d,stack len=%d\n", *v, stack_len(&sk));
  }
  free(arr);
  stack_deinit(&sk);
  _Atomic(size_t) count = ATOMIC_VAR_INIT(0);
  thrd_t thd[2];
  for (int i = 0; i < 2; i++)
  {
    thrd_create(&thd[i], (thrd_start_t)& sum, (void *)&count);
  }
  for (int i = 0; i < 2; i++)
  {
    thrd_join(thd[i], NULL);
  }
  fprintf(stdout, "count=%d\n", count);
}
#endif