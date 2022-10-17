/*
 * File            : kfuse.c
 * Author          : ZhangLe
 * CreateTime      : 2022-10-17 11:36:11
 * LastModified    : 2022-10-17 16:01:16
 * Vim             : ts=4, sw=4
 */

#include <stdio.h>
#include <stdlib.h>
#include "MonMap.h"

int main(int argc, const char** argv){
    // load monmap
    MonMap* monmap;
    monmap = (MonMap *)malloc(sizeof(MonMap));
    monmap_init(monmap);
    monmap_read(monmap, ".kirinfs");
    free(monmap);
    return 0;
}
