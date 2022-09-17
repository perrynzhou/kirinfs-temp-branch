/*************************************************************************
    > File Name: stl_rwlock.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 12 Sep 2022 04:06:19 AM EDT
 ************************************************************************/

#ifndef _STL_RWLOCK_H
#define _STL_RWLOCK_H
#include <stdint.h>
typedef volatile uint64_t stl_rwlock;

int stl_rwlock_wlock(stl_rwlock *lock);
int stl_rwlock_rlock(stl_rwlock *lock);
int stl_rwlock_unlock(stl_rwlock *lock);
int stl_rwlock_downgrade(stl_rwlock *lock);
#endif
