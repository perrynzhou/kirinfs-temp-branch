/*************************************************************************
    > File Name: c_binary_heap.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 28 Mar 2022 11:21:25 AM UTC
 ************************************************************************/

#include "inc/c_binary_heap.h"
#define C_BINARY_HEAP_INCR (32)
c_binary_heap_t *c_binary_heap_alloc(size_t mem_size, c_binary_heap_compare_cb cmp_cb, c_binary_heap_free_cb free_cb)
{
   c_binary_heap_t *heap = (c_binary_heap_t *)calloc(1, sizeof(c_binary_heap_t));
   if (heap != NULL)
   {
      if (c_binary_heap_init(heap, mem_size, cmp_cb, free_cb) != -1)
      {
         return heap;
      }
      else
      {
         free(heap);
         heap = NULL;
      }
   }
   return NULL;
}

int c_binary_heap_init(c_binary_heap_t *heap, size_t mem_size, c_binary_heap_compare_cb cmp_cb, c_binary_heap_free_cb free_cb)
{
   if (heap != NULL && mem_size > 0 && cmp_cb != NULL)
   {
      heap->ptr = (void **)calloc(C_BINARY_HEAP_INCR, sizeof(void *));
      assert(heap->ptr != NULL);
      heap->cmp_cb = cmp_cb;
      heap->free_cb = free_cb;
      heap->cur_count = 0;
      heap->cap_count = C_BINARY_HEAP_INCR;
      return 0;
   }
   return -1;
}
int c_binary_heap_push(c_binary_heap_t *heap, void *data)
{
   if (heap != NULL && data != NULL)
   {
      if (heap->cur_count >= heap->cap_count)
      {
         size_t expand_size = heap->cap_count + 2 * C_BINARY_HEAP_INCR;
         heap->ptr = (void **)realloc(heap->ptr, sizeof(void *) * expand_size);
         if (heap->ptr != NULL)
         {
            return -1;
         }
         heap->cap_count = expand_size;
      }
      size_t index = heap->cur_count;
      void *ptr = calloc(1, sizeof(heap->mem_size) * sizeof(char));
      memcpy(ptr, data, heap->mem_size);
      heap->cur_count++;
      size_t parent = 0;
      while (index > 0)
      {
         parent = (index - 1) / 2;
         if (heap->cmp_cb(heap->ptr[parent], ptr) < 0)
         {
            break;
         }
         else
         {
            heap->ptr[index] = heap->ptr[parent];
            index = parent;
         }
      }
      heap->ptr[index] = ptr;
      return 0;
   }
   return -1;
}
void *c_binary_heap_pop(c_binary_heap_t *heap)
{
   void *data = NULL;
   if (heap != NULL && heap->cur_count > 0)
   {
      data = heap->ptr[0];
      void *new_data = heap->ptr[--heap->cur_count];
      size_t index = 0, next_index;
      size_t child1, child2;
      for (;;)
      {
         child1 = index * 2 + 1;
         child2 = index * 2 + 2;
         if (child1 < heap->cur_count && heap->cmp_cb(new_data, heap->ptr[child1]) > 0)
         {
            if (child2 < heap->cur_count && heap->cmp_cb(heap->ptr[child1], heap->ptr[child2]) > 0)
            {
               next_index = child2;
            }
            else
            {
               next_index = child1;
            }
         }
         else if (child2 < heap->cur_count && heap->cmp_cb(new_data, heap->ptr[child2]) < 0)
         {
            next_index = child2;
         }
         else
         {
            heap->ptr[index] = new_data;
            break;
         }
         heap->ptr[index] = heap->ptr[next_index];
         index = next_index;
      }
   }
   return data;
}
void c_binary_heap_deinit(c_binary_heap_t *heap)
{
   if(heap!=NULL && heap->cur_count>0)
   {
       bool is_set_free = (heap->free_cb==NULL)?(false):(true);
      for(size_t i=0;i<heap->cur_count;i++)
      {
         if(is_set_free) {
            heap->free_cb(heap->ptr[i]);
         }
      }
      heap->cur_count = 0;
   }
}
int  c_binary_heap_traverse(c_binary_heap_t *heap,c_binary_heap_traverse_cb traverse_cb)
{
   int ret = -1;
     if(heap!=NULL && heap->cur_count>0)
   {
       bool is_set_cb = (traverse_cb==NULL)?(false):(true);
      for(size_t i=0;i<heap->cur_count;i++)
      {
         if(is_set_cb&& (ret=traverse_cb(heap->ptr[i])) !=0) {
            break;
         }
      }
      heap->cur_count = 0;
   }
   return ret;
}
void c_binary_heap_destroy(c_binary_heap_t *heap) 
{
   c_binary_heap_deinit(heap);
   if(heap !=NULL) {
      free(heap->ptr);
      free(heap);
      heap=NULL;
   }
}