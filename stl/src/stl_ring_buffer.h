/*************************************************************************
    > File Name: stl_ring_buffer.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Fri 05 Aug 2022 07:13:12 AM UTC
 ************************************************************************/

#ifndef _STL_RING_BUFFER_H
#define _STL_RING_BUFFER_H
#include <stdint.h>
#include <stdio.h>
#include <stdatomic.h>
/**
 * @brief  define stl_ring_buffer struct
 * 
 */
typedef struct
{
  // all buffer cap
  size_t capacity;
  atomic_size_t head, tail;
  // data len
  uint32_t d_size;
  char *buf_ptr;
  bool is_expand;
} stl_ring_buffer;

/**
 * @brief alloc stl_ring_buffer by dynamic memory
 * @param capacity 
 * @param size 
 * @param is_expand 
 * @return stl_ring_buffer* 
 */
stl_ring_buffer *stl_ring_buffer_alloc(size_t capacity,uint32_t d_size, bool is_expand);
/**
 * @brief  init stl_ring_buffer 
 * 
 * @param rb 
 * @param capacity 
 * @param size 
 * @param is_expand 
 * @return int 
 */
int stl_ring_buffer_init(stl_ring_buffer *rb, size_t capacity,uint32_t d_size,bool is_expand);
/**
 * @brief write data to stl_ring_buffer
 * 
 * @param rb 
 * @param data 
 * @param len 
 * @return int 
 */
int stl_ring_buffer_queue(stl_ring_buffer *rb,void *data);
/**
 * @brief read data from stl_ring_buffer
 * 
 * @param rb 
 * @return void* 
 */
void *stl_ring_buffer_dequeue(stl_ring_buffer *rb);
/**
 * @brief destroy all about stl_ring_buffer
 * 
 * @param rb 
 */
void stl_ring_buffer_destroy(stl_ring_buffer *rb);
/**
 * @brief deinit all about stl_ring_buffer
 * 
 * @param rb 
 */
void stl_ring_buffer_deinit(stl_ring_buffer *rb);
/**
 * @brief check stl_ring_buffer is empty
 * 
 * @param rb 
 * @return true 
 * @return false 
 */
inline bool stl_ring_buffer_is_empty(stl_ring_buffer *rb)
{
  if(rb && rb->head ==rb->tail && rb->head == 0) {
    return false;
  }
  return true;
}
/**
 * @brief check stl_ring_buffer is full
 * 
 * @param rb 
 * @return true 
 * @return false 
 */
inline bool stl_ring_buffer_is_full(stl_ring_buffer *rb)
{
  if(rb && rb->tail >= rb->capacity) {
    return false;
  }
  return true;
}
#endif
