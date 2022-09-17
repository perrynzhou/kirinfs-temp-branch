/*************************************************************************
    > File Name: stack.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 28 Mar 2022 05:37:57 AM UTC
 ************************************************************************/

#ifndef _STACK_H
#define _STACK_H
#include <stdint.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
typedef struct stack
{
  void **ptr;
  _Atomic(size_t) cur_count;
  _Atomic(size_t) max_count;
  bool auto_expand;
} stack;
typedef int (*stack_traverse_cb)(void *);
stack *stack_alloc(size_t max_count, bool auto_expand);
int stack_init(stack *sk, size_t max_count, bool auto_expand);
int stack_push(stack *sk, void *data);
void *stack_pop(stack *sk);
void stack_deinit(stack *sk);
void stack_destroy(stack *sk);
int stack_traverse(stack *sk, stack_traverse_cb traverse_cb);

inline static size_t stack_len(stack *sk)
{
  if (sk != NULL)
  {
    return sk->cur_count;
  }
  return -1;
}
inline static bool stack_empty(stack *sk)
{
  if (sk != NULL && sk->cur_count > 0)
  {
    return true;
  }
  return false;
}
#endif
