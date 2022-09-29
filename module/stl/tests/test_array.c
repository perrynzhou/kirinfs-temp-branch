/*************************************************************************
    > File Name: test_array.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 26 Mar 2022 09:15:39 AM UTC
 ************************************************************************/

#include <stdio.h>
#include "../src/c_array.h"
int array_traverse_cb(void *data)
{
  char *value = (char *)data;
  fprintf(stdout, "data=%s,addr=%p\n", value, value);
  return 0;
}

int main()
{
  c_array_t arr;
  size_t n = 5;
  c_array_init(&arr, sizeof(char *), 9, NULL);
  {
    //int datas[5] = {1, 2, 3, 4, 5};
    char *datas[5] ={"a","b","c","d","e"};
    for (int i = 0; i < n; i++)
    {
      c_array_add(&arr, datas[i]);
    }
    fprintf(stdout, "*****c_array_add*******\n");

    c_array_traverse(&arr, array_traverse_cb);
    char *new = "9";
    fprintf(stdout, "*****c_array_add_at*******\n");
    c_array_add_at(&arr, new, 0);
    c_array_traverse(&arr, array_traverse_cb);
    fprintf(stdout, "******c_array_remove_at****\n");
    c_array_remove_at(&arr, 2);
    c_array_traverse(&arr, array_traverse_cb);
    fprintf(stdout, "******c_array_remove****\n");

    c_array_remove(&arr);
    c_array_traverse(&arr, array_traverse_cb);

    fprintf(stdout, "******c_array_swap****\n");

    c_array_swap(&arr, 0, 1);
    c_array_traverse(&arr, array_traverse_cb);
    fprintf(stdout, "******c_array_replace****\n");
    char *r_value = "100";
    c_array_replace(&arr, 0, r_value);
    c_array_traverse(&arr, array_traverse_cb);
  }
    c_array_deinit(&arr);
    fprintf(stdout, "******c_array_deinit****\n");
    c_array_traverse(&arr, array_traverse_cb);
}