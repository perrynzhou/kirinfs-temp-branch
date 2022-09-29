/*************************************************************************
  > File Name: test_string.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: 日  7/24 17:37:31 2022
 ************************************************************************/

#include "../src/stl_string.h"
#include <stdio.h>
int main(int argc, char *arv[])
{
        stl_string  *str = stl_string_alloc("a d d N M");
        char *data = stl_string_data(str);
        fprintf(stdout," trim before:%s\n",data);
        stl_string_trim(str);
        data = stl_string_data(str);
        fprintf(stdout," trim after:%s\n",data);
        stl_string_destroy(str);
}