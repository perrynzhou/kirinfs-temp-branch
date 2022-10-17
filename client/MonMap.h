/*
 * File            : MonMap.h
 * Author          : ZhangLe
 * CreateTime      : 2022-10-17 11:37:13
 * LastModified    : 2022-10-17 16:01:15
 * Vim             : ts=4, sw=4
 */

#ifndef _MONMAP_H_
#define _MONMAP_H_

typedef struct MonMap{
    int num;
    int num_mon;
    int mon_inst;
    int last_mon;
} MonMap;

void monmap_init(struct MonMap* monmap);
int monmap_read(struct MonMap* mp, const char* fn);
#endif    /* MonMap.h */
