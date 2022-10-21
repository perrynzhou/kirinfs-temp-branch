/*
 * File            : SimpleMessenger.c
 * Author          : ZhangLe
 * CreateTime      : 2022-10-21 15:58:44
 * LastModified    : 2022-10-21 16:36:34
 * Vim             : ts=4, sw=4
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "SimpleMessenger.h"

int Rank_init(struct Rank* rank){
    rank->started = false;
    rank->max_local = 0;
    rank->num_local = 0;
    rank->accepter = (struct Accepter *)malloc(sizeof(struct Accepter));
    rank->accepter->done = false;
    return 0;
}

int Rank_accepter_bind(struct Rank* rank){
    printf("accepter.bind\n");

    char hostname[100];
    memset(hostname, 0, 100);
    gethostname(hostname, 100);

    printf("accepter.bind my hostname is %s\n", hostname);

    // todo is there a .ceph_hosts file?

    return 0;
}

int Rank_bind(struct Rank* rank){
    if(rank->started){
        printf("rank.bind already started\n");
        return 0;
    }
    return Rank_accepter_bind(rank);
}

int main(){
    struct Rank* rank = (struct Rank *)malloc(sizeof(struct Rank));
    printf("started %d\n", rank->started);
    printf("max_local %d\n", rank->started);
    Rank_bind(rank);
    return 0;
}
