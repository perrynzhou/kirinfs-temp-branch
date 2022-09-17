/*************************************************************************
  > File Name: stl_thread_pool.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Thu 04 Aug 2022 01:02:09 PM CST
 ************************************************************************/
#define _GNU_SOURCE
#include "stl_thread_pool.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
static volatile int threads_keepalive;
static volatile int threads_on_hold;

/* Binary semaphore */
typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int v;
} bsem;

typedef struct task
{

    struct task *prev;               /* pointer to previous job   */
    void (*do_task_func)(void *arg); /* function pointer          */
    void *arg;                       /* function's argument       */
} task;

/* task queue */
typedef struct
{
    int tid;
    pthread_mutex_t rwmutex; /* used for queue r/w access */
    task *front;             /* pointer to front of queue */
    task *rear;              /* pointer to rear  of queue */
    bsem *has_jobs;          /* flag as binary semaphore  */
    int len;                 /* number of jobs in queue   */

} task_queue;
static task *task_queue_pull(void *arg);
inline static task *task_alloc(stl_thread_do_task_func func, void *arg_p)
{
    task *new_task = calloc(1, sizeof(task));
    assert(new_task != NULL);
    if (func && arg_p)
    {
        new_task->do_task_func = func;
        new_task->arg = arg_p;
    }
    return new_task;
}
inline static void task_destroy(task *new_task)
{
    if (new_task)
    {
        free(new_task);
        new_task = NULL;
    }
}
/* ======================== SYNCHRONISATION ========================= */

/* Init semaphore to 1 or 0 */
static void bsem_init(bsem *bsem_p, int value)
{
    if (value < 0 || value > 1)
    {
        exit(1);
    }
    pthread_mutex_init(&(bsem_p->mutex), NULL);
    pthread_cond_init(&(bsem_p->cond), NULL);
    bsem_p->v = value;
}

/* Reset semaphore to 0 */
static void bsem_reset(bsem *bsem_p)
{
    bsem_init(bsem_p, 0);
}

/* Post to at least one thread */
static void bsem_post(bsem *bsem_p)
{
    pthread_mutex_lock(&bsem_p->mutex);
    bsem_p->v = 1;
    pthread_cond_signal(&bsem_p->cond);
    pthread_mutex_unlock(&bsem_p->mutex);
}

/* Post to all threads */
static void bsem_post_all(bsem *bsem_p)
{
    pthread_mutex_lock(&bsem_p->mutex);
    bsem_p->v = 1;
    pthread_cond_broadcast(&bsem_p->cond);
    pthread_mutex_unlock(&bsem_p->mutex);
}

/* Wait on semaphore until semaphore has value 0 */
static void bsem_wait(bsem *bsem_p)
{
    pthread_mutex_lock(&bsem_p->mutex);
    while (bsem_p->v != 1)
    {
        pthread_cond_wait(&bsem_p->cond, &bsem_p->mutex);
    }
    bsem_p->v = 0;
    pthread_mutex_unlock(&bsem_p->mutex);
}

/* ============================ THREAD ============================== */
static void *stl_thread_do(stl_thread *thread_p);
static int stl_thread_init(stl_thread_pool *tp, stl_thread **thread_p, int id)
{

    *thread_p = (stl_thread *)malloc(sizeof(stl_thread));
    if (*thread_p == NULL)
    {
        return -1;
    }

    (*thread_p)->pool = tp;
    (*thread_p)->id = id;

    pthread_create(&(*thread_p)->pthread, NULL, (void *(*)(void *))stl_thread_do, (*thread_p));
    pthread_detach((*thread_p)->pthread);
    return 0;
}

/* Sets the calling thread on hold */
static void stl_thread_hold(int sig_id)
{
    (void)sig_id;
    threads_on_hold = 1;
    while (threads_on_hold)
    {
        alarm(1);
    }
}

inline static int stl_thread_bind_cpu(stl_thread *thread_p)
{
    long cpu_cnt = sysconf(_SC_NPROCESSORS_CONF);
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    int cpu_core_id = thread_p->id % cpu_cnt;
    CPU_SET(cpu_core_id, &cpuset);
    pthread_t thid = pthread_self();
    return pthread_setaffinity_np(thid, sizeof(cpu_set_t), &cpuset);
}
static void *stl_thread_do(stl_thread *thread_p)
{

    /* set thread name */
    char thread_name[64] = {0};
    snprintf(thread_name, 32, "thread-pool-%d", thread_p->id);
    pthread_setname_np(thread_p->pthread, (char *)&thread_name);

    // bind thread to cpu core
    stl_thread_bind_cpu(thread_p);
    stl_thread_pool *tp = thread_p->pool;

    /* Register signal handler */
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = stl_thread_hold;
    if (sigaction(SIGUSR1, &act, NULL) == -1)
    {
        return NULL;
    }

    atomic_fetch_add(&tp->num_threads_alive, 1);

    while (threads_keepalive)
    {
        task_queue *t_queue = (task_queue *)tp->task_queue[thread_p->id];
        bsem_wait(t_queue->has_jobs);

        if (threads_keepalive)
        {

            atomic_fetch_add(&tp->num_threads_working, 1);
            task *task_p = task_queue_pull(tp->task_queue[thread_p->id]);
            if (task_p)
            {
                task_p->do_task_func(task_p->arg);
                atomic_fetch_add(&thread_p->done_task_num, 1);
                task_destroy(task_p);
            }

            atomic_fetch_sub(&tp->num_threads_working, 1);
            if (!tp->num_threads_working)
            {
                pthread_cond_signal(&tp->threads_all_idle);
            }
        }
    }
    atomic_fetch_sub(&tp->num_threads_alive, 1);
    pthread_exit(NULL);
    return NULL;
}

/* Frees a thread  */
static void stl_thread_destroy(stl_thread *thread_p)
{
    free(thread_p);
}

/* ======================== TASK_QUEUE ========================= */
static task_queue *task_queue_alloc(size_t n)
{
    task_queue *task_queue_p = (task_queue *)calloc(1, sizeof(task_queue));
    assert(task_queue_p != NULL);

    task_queue_p->len = ATOMIC_VAR_INIT(0);
    task_queue_p->front = NULL;
    task_queue_p->rear = NULL;
    task_queue_p->tid = n;
    task_queue_p->has_jobs = (bsem *)malloc(sizeof(bsem));
    assert(task_queue_p->has_jobs != NULL);

    pthread_mutex_init(&task_queue_p->rwmutex, NULL);
    bsem_init(task_queue_p->has_jobs, 0);

    return task_queue_p;
}

/* Clear the queue */
static void task_queue_clear(void *arg)
{

    task_queue *task_queue_p = (task_queue *)arg;
    while (task_queue_p->len)
    {
        free(task_queue_pull(task_queue_p));
    }

    task_queue_p->front = NULL;
    task_queue_p->rear = NULL;
    bsem_reset(task_queue_p->has_jobs);
    task_queue_p->len = 0;
}

/* Add (allocated) job to queue
 */
static void task_queue_push(void *arg, struct task *new_task)
{

    task_queue *task_queue_p = (task_queue *)arg;
    pthread_mutex_lock(&task_queue_p->rwmutex);
    new_task->prev = NULL;
    // fprintf(stdout,"task_queue-%ld add paramete=%ld\n",task_queue_p->tid,*(int *)new_task->arg);
    switch (task_queue_p->len)
    {

    case 0: /* if no jobs in queue */
        task_queue_p->front = new_task;
        task_queue_p->rear = new_task;
        break;

    default: /* if jobs in queue */
        task_queue_p->rear->prev = new_task;
        task_queue_p->rear = new_task;
    }
    task_queue_p->len++;

    bsem_post(task_queue_p->has_jobs);
    pthread_mutex_unlock(&task_queue_p->rwmutex);
}

static task *task_queue_pull(void *arg)
{

    task_queue *task_queue_p = (task_queue *)arg;
    pthread_mutex_lock(&task_queue_p->rwmutex);
    task *task_p = task_queue_p->front;

    switch (task_queue_p->len)
    {

    case 0: /* if no jobs in queue */
        break;

    case 1: /* if one job in queue */
        task_queue_p->front = NULL;
        task_queue_p->rear = NULL;
        task_queue_p->len = 0;
        break;

    default: /* if >1 jobs in queue */
        task_queue_p->front = task_p->prev;
        task_queue_p->len--;
        /* more than one job in queue -> post it */
        bsem_post(task_queue_p->has_jobs);
    }

    pthread_mutex_unlock(&task_queue_p->rwmutex);
    return task_p;
}

static void task_queue_destroy(void *arg)
{
    task_queue *task_queue_p = (task_queue *)arg;
    task_queue_clear(task_queue_p);
    free(task_queue_p->has_jobs);
}

stl_thread_pool *stl_thread_pool_alloc(int num_threads)
{

    /* Make new thread pool */
    stl_thread_pool *tp = (stl_thread_pool *)malloc(sizeof(stl_thread_pool));
    assert(tp != NULL);
    if (stl_thread_pool_init(tp, num_threads) != 0)
    {
        free(tp);
        tp = NULL;
    }
    return tp;
}

int stl_thread_pool_init(stl_thread_pool *tp, int num_threads)
{
    if (tp != NULL && num_threads > 0)
    {
        threads_on_hold = 0;
        threads_keepalive = 1;

        if (num_threads < 0)
        {
            num_threads = 1;
        }
        tp->num_threads_alive = 0;
        tp->num_threads_working = 0;
        tp->num_threads = num_threads;
        tp->task_queue = calloc(num_threads, sizeof(task_queue *));
        assert(tp->task_queue != NULL);
        /* Make threads in pool */
        tp->threads = (stl_thread **)calloc(num_threads, sizeof(stl_thread *));
        assert(tp->threads != NULL);

        pthread_mutex_init(&(tp->thcount_lock), NULL);
        pthread_cond_init(&tp->threads_all_idle, NULL);

        int n;
        for (n = 0; n < num_threads; n++)
        {

            tp->task_queue[n] = task_queue_alloc(n);
            stl_thread_init(tp, &tp->threads[n], n);
        }
    }
    return -1;
}

int stl_thread_pool_add_task(stl_thread_pool *tp, stl_thread_do_task_func func, void *arg_p)
{
    task *new_task = task_alloc(func, arg_p);

    atomic_fetch_add(&tp->his_task_num, 1);
    size_t index = tp->his_task_num % tp->num_threads;
    task_queue_push(tp->task_queue[index], new_task);
    return 0;
}
void stl_thread_pool_dump(stl_thread_pool *tp)
{
    if (tp)
    {
        for (size_t i = 0; i < tp->num_threads; i++)
        {
            fprintf(stdout, "thread-%ld done task:%ld\n", i, tp->threads[i]->done_task_num);
        }
    }
}
void stl_thread_pool_wait(stl_thread_pool *tp)
{
    pthread_mutex_lock(&tp->thcount_lock);
    task_queue **queue = (task_queue **)tp->task_queue;

    for (size_t i = 0; i < tp->num_threads; i++)
    {
        while (queue[i]->len || tp->num_threads_working)
        {
            pthread_cond_wait(&tp->threads_all_idle, &tp->thcount_lock);
        }
    }
    pthread_mutex_unlock(&tp->thcount_lock);
}
int stl_thread_pool_running_threads(stl_thread_pool *tp)
{
    return tp->num_threads_working;
}
void stl_thread_pool_deinit(stl_thread_pool *tp)
{
    /* No need to destory if it's NULL */
    if (tp == NULL)
        return;

    volatile int threads_total = tp->num_threads_alive;

    /* End each thread 's infinite loop */
    threads_keepalive = 0;

    /* Give one second to kill idle threads */
    double TIMEOUT = 1.0;
    time_t start, end;
    double tpassed = 0.0;
    time(&start);
    task_queue *queue = (task_queue *)tp->task_queue;
    while (tpassed < TIMEOUT && tp->num_threads_alive)
    {
        bsem_post_all(queue->has_jobs);
        time(&end);
        tpassed = difftime(end, start);
    }

    /* Poll remaining threads */
    while (tp->num_threads_alive)
    {
        bsem_post_all(queue->has_jobs);
        alarm(1);
    }

    for (size_t i = 0; i < tp->num_threads; i++)
    {
        task_queue_destroy(tp->task_queue[i]);
    }
    free(tp->task_queue);

    for (int n = 0; n < threads_total; n++)
    {
        stl_thread_destroy(tp->threads[n]);
    }
    free(tp->threads);
}
void stl_thread_pool_destroy(stl_thread_pool *tp)
{
    if (tp != NULL)
    {
        stl_thread_pool_deinit(tp);
    }
}