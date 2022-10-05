/*************************************************************************
    > File Name: stl_rwlock.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 12 Sep 2022 04:06:23 AM EDT
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include "stl_rwlock.h"

#define STL_RWLOCK_SPIN 2048
#define STL_RWLOCK_WLOCK_MODE ((uint64_t)-1)

#define stl_memory_barrier() __asm__ volatile("" :: \
                                                  : "memory")

#define stl_cpu_pause() __asm__("pause")

#if (STL_SMP)
#define STL_SMP_LOCK "lock;"
#else
#define STL_SMP_LOCK
#endif

static intptr_t stl_ncpu;

static inline int64_t
stl_atomic_fetch_add(stl_rwlock *value, int64_t add)
{
  __asm__ volatile(

      STL_SMP_LOCK
      "    xaddq  %0, %1;   "

      : "+r"(add)
      : "m"(*value)
      : "cc", "memory");

  return add;
}

static inline uint64_t stl_atomic_cmp_set(stl_rwlock *lock, uint64_t old,
                                          uint64_t set)
{
  unsigned char res;
  __asm__ volatile(
      STL_SMP_LOCK
      "    cmpxchgq  %3, %1;   "
      "    sete      %0;       "

      : "=a"(res)
      : "m"(*lock), "a"(old), "r"(set)
      : "cc", "memory");

  return res;
}

int stl_rwlock_wlock(stl_rwlock *lock)
{
  if (lock)
  {
    if (!stl_ncpu)
    {
      stl_ncpu = sysconf(_SC_NPROCESSORS_ONLN);
    }
    uintptr_t i, n;

    for (;;)
    {

      if (*lock == 0 && stl_atomic_cmp_set(lock, 0, STL_RWLOCK_WLOCK_MODE))
      {
        return;
      }

      if (stl_ncpu > 1)
      {

        for (n = 1; n < STL_RWLOCK_SPIN; n <<= 1)
        {

          for (i = 0; i < n; i++)
          {
            stl_cpu_pause();
          }

          if (*lock == 0 && stl_atomic_cmp_set(lock, 0, STL_RWLOCK_WLOCK_MODE))
          {
            return;
          }
        }
      }

      stl_sched_yield();
    }
    return 0;
  }
  return -1;
}

int stl_rwlock_rlock(stl_rwlock *lock)
{
  if (lock)
  {
    uintptr_t i, n;
    uint64_t readers;
    if (!stl_ncpu)
    {
      stl_ncpu = sysconf(_SC_NPROCESSORS_ONLN);
    }
    for (;;)
    {
      readers = *lock;

      if (readers != STL_RWLOCK_WLOCK_MODE && stl_atomic_cmp_set(lock, readers, readers + 1))
      {
        return;
      }

      if (stl_ncpu > 1)
      {

        for (n = 1; n < STL_RWLOCK_SPIN; n <<= 1)
        {

          for (i = 0; i < n; i++)
          {
            stl_cpu_pause();
          }

          readers = *lock;

          if (readers != STL_RWLOCK_WLOCK_MODE && stl_atomic_cmp_set(lock, readers, readers + 1))
          {
            return;
          }
        }
      }

      stl_sched_yield();
    }
    return 0;
  }
  return -1;
}

int stl_rwlock_unlock(stl_rwlock *lock)
{
  if (lock)
  {
    if (*lock == STL_RWLOCK_WLOCK_MODE)
    {
      (void)stl_atomic_cmp_set(lock, STL_RWLOCK_WLOCK_MODE, 0);
    }
    else
    {
      (void)stl_atomic_fetch_add(lock, -1);
    }
    return 0;
  }
  return -1;
}

int stl_rwlock_downgrade(stl_rwlock *lock)
{
  if (lock)
  {
    if (*lock == STL_RWLOCK_WLOCK_MODE)
    {
      *lock = 1;
    }
    return 0;
  }
  return -1;
}