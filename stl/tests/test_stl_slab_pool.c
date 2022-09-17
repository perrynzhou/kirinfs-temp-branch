/*************************************************************************
    > File Name: test_stl_slab_pool.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Tue 29 Mar 2022 01:45:48 AM UTC
 ************************************************************************/

#include "../src/stl_slab_pool.h"
#include <assert.h>
#include <string.h>
#include <unistd.h>
int main()
{
   stl_slab_pool pool;
   stl_slab_pool_init(&pool, 1024 * 1024);
   size_t succ = 0;
   for (size_t n = 1; n < 40960; n++)
   {
      for (size_t i = 0; i < 100; i++)
      {
         size_t n = rand() % 3 + (rand() - 100) % 7 + i;
         void *p1 = stl_slab_pool_alloc(&pool, n);
         assert(p1 != NULL);
         stl_slab_pool_free(&pool, p1);
         memset(p1, 0, n);
         succ++;
      }
   }
   fprintf(stdout, "succ=%ld\n", succ);
   return 0;
}