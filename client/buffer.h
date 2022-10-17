/*
 * File            : buffer.h
 * Author          : ZhangLe
 * CreateTime      : 2022-10-17 14:28:40
 * LastModified    : 2022-10-17 15:25:08
 * Vim             : ts=4, sw=4
 */

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdio.h>

struct raw {
    char* data;
};

struct buffer_ptr{
    struct raw* _raw;
    unsigned _off, _len;
};

void buffer_ptr_init(struct buffer_ptr* bp, unsigned l);
void* buffer_ptr_c_str(struct buffer_ptr* bp);

#endif    /* buffer.h */
