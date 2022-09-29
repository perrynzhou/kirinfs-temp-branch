/*************************************************************************
    > File Name: test_stl_rand.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 05 Sep 2022 11:17:16 PM EDT
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include "../src/stl_rand.h"
int main()
{
  for (size_t i = 0; i < 10; i++)
  {
    int32_t r = stl_rand_int32();
    fprintf(stdout, "r=%d, ", r);
  }
  fprintf(stdout, "*******************\n");
  char buf[64] = {'\0'};
  for (size_t i = 0; i < 10; i++)
  {
    int ret = stl_rand_string((char *)&buf, 60);
    fprintf(stdout, "stl_rand_string ret=%d,len=%d,s=%s\n", ret, strlen((char *)&buf), (char *)&buf);
  }

  for (size_t i = 0; i < 10; i++)
  {
    int ret = stl_gen_uuid((char *)&buf, 60);
    fprintf(stdout, "stl_gen_uuid ret=%d,len=%d,s=%s\n", ret, strlen((char *)&buf), (char *)&buf);
  }

  return 0;
}
