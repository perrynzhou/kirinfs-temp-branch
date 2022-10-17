/*
 * File            : buffer.c
 * Author          : ZhangLe
 * CreateTime      : 2022-10-17 14:48:12
 * LastModified    : 2022-10-17 15:24:21
 * Vim             : ts=4, sw=4
 */

#include <stdlib.h>
#include "buffer.h"

void buffer_ptr_init(struct buffer_ptr* bp, unsigned l) {
    bp->_raw = (struct raw*)malloc(sizeof(struct raw));
    bp->_off = 0;
    bp->_len = l;
}

void* buffer_ptr_c_str(struct buffer_ptr* bp) {
    bp->_raw->data = (char *)malloc(sizeof(bp->_len));
    return bp->_raw->data + bp->_off;
}
