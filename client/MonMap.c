/*
 * File            : MonMap.c
 * Author          : ZhangLe
 * CreateTime      : 2022-10-17 11:39:42
 * LastModified    : 2022-10-17 16:01:15
 * Vim             : ts=4, sw=4
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "MonMap.h"
#include "buffer.h"

void monmap_init(struct MonMap* mp) {
    mp->num_mon = 0;
    mp->mon_inst = 0;
    mp->last_mon = -1;
}

void monmap_decode(struct MonMap* mp, struct buffer_ptr* bp){
    // ...
    mp->num_mon = 0;
    mp->mon_inst = 1;
    mp->last_mon = 1;
}

int monmap_read(struct MonMap* mp, const char* fn) {
    struct stat st;
    struct buffer_ptr *bp = (struct buffer_ptr*)malloc(sizeof(struct buffer_ptr));

    int fd = open(fn, O_RDONLY);
    if (fd < 0)
        return fd;
    fstat(fd, &st);
    buffer_ptr_init(bp, st.st_size);
    read(fd, (void*)buffer_ptr_c_str(bp), bp->_len);
    monmap_decode(mp, bp);
    close(fd);
    return 0;
}
