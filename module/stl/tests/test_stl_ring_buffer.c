#include <stdio.h>
#include <stdbool.h>
#include "../src/stl_ring_buffer.h"

int main()
{
    int i,nums = 20;
    int arr[nums];
    stl_ring_buffer buffer;
    stl_ring_buffer_init(&buffer, nums, sizeof(int), false);
    for(i = 0; i < nums + 20; ++i) {
        if(stl_ring_buffer_is_full(&buffer)){
            printf("index=%d buffer is full\n", i);
            break;
        }
        arr[i] = i;
        stl_ring_buffer_queue(&buffer, &arr[i]);
    }

    int *tmp;
    for(i = 0; i < nums; ++i) {
        tmp = stl_ring_buffer_dequeue(&buffer);
        if(tmp != NULL) {
            printf("val = %d\n", *tmp);
        } else {
            printf("tmp == NULL\n");
            break;
        }
    }

    stl_ring_buffer_deinit(&buffer);
}