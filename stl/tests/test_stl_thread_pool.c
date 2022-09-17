/*************************************************************************
  > File Name: test_string.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: 日  7/24 17:37:31 2022
 ************************************************************************/

#include "../src/stl_thread_pool.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
void *stl_thread_fn(void *arg)
{
        size_t *data = (size_t *)arg;
        fprintf(stdout, "i=%ld\n", *data);
        return NULL;
}
int main(int argc, char *arv[])
{
        size_t n = 10;
        size_t *data = calloc(n, sizeof(size_t));

        stl_thread_pool tp;
        stl_thread_pool_init(&tp, n);
        for (size_t i = 0; i < n; i++)
        {
                size_t index = i;
                data[index] = i;
                for (size_t j = 0; j < 100; j++)
                {
                        stl_thread_pool_add_task(&tp, (stl_thread_do_task_func)stl_thread_fn, &data[index]);
                }
        }
        stl_thread_pool_wait(&tp);
        stl_thread_pool_dump(&tp);
        return 0;
}