/*************************************************************************
    > File Name: test_vector.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Sat 26 Mar 2022 12:02:00 PM UTC
 ************************************************************************/

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include "../src/inc/c_vector.h"
int vec_print(void *data)
{
  fprintf(stdout, "%s\n", (char *)data);

  return 0;
}
int vec_print2(void *data)
{

  fprintf(stdout, "%d\n", *(int *)data);
  return 0;
}
int vec_free(void *data)
{
  if (data != NULL)
  {
    free(data);
    data = NULL;
  }
}
int main()
{
  int n = 3;
  c_vector_t vec1;
  c_vector_init(&vec1, sizeof(char *), NULL);
  // c_vector_t *c_vector_alloc(size_t mem_size,c_data_type_t d_type, c_vector_free_func_cb free_cb);

  c_vector_t *vec2 = c_vector_alloc(sizeof(int), NULL);
  for (int i = 0; i < n; i++)
  {
    char buf[256] = {'\0'};
    snprintf((char *)&buf, 256, "key-%d", i);
    c_vector_add(&vec1, &buf);
    c_vector_traverse(&vec1, vec_print);
    c_vector_add(vec2, &i);
  }
  c_vector_traverse(vec2, vec_print2);

  c_vector_deinit(&vec1);
  c_vector_destroy(vec2);
}