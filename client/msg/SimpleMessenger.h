/*
 * File            : SimpleMessenger.h
 * Author          : ZhangLe
 * CreateTime      : 2022-10-21 15:49:41
 * LastModified    : 2022-10-21 16:33:04
 * Vim             : ts=4, sw=4
 */

#ifndef _SIMPLEMESSENGER_H_
#define _SIMPLEMESSENGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// incoming
struct Accepter{
    _Bool done;
    int listen_sd;
};

/* Rank - per-process
 */

struct Rank{
    _Bool started;
    unsigned max_local, num_local;
    struct Accepter* accepter;
};

int Rank_init(struct Rank* rank);
int Rank_accepter_bind(struct Rank* rank);
int Rank_bind(struct Rank* rank)

#endif    /* SimpleMessenger.h */
