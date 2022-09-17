/*************************************************************************
    > File Name: test_log.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Tue 29 Mar 2022 01:45:48 AM UTC
 ************************************************************************/

#include "../src/inc/c_log.h"
int main() {
    size_t n = 100;
    for(size_t i=0;i<n;i++) {
       logi(" data=%d,fucn data=%s",i,"asdfs");
    }
    return 0;
}