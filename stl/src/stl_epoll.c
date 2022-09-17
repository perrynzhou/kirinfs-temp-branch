/*************************************************************************
    > File Name: stl_epoll.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Wed 07 Sep 2022 05:06:14 AM UTC
 ************************************************************************/

#include "stl_epoll.h"
#include "stl_socket.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <arpa/inet.h>
#define STL_EPOLL_INIT_EVENT_SIZE 512
extern inline int stl_epoll_set_context(stl_epoll *sep, void *ctx);
extern inline int stl_epoll_thread_num(stl_epoll *sep);
extern  inline int stl_epoll_stop(stl_epoll *sep);
static void *stl_epoll_thread_func(void *args);
static void stl_epoll_add_event(int efd, int fd)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLERR;
    // ev.events =  EPOLLIN  | EPOLLERR ;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev) < 0)
    {
        perror("stl_epoll_add_event");
    }
}

static void stl_epoll_modify_event(int efd, int fd, int state)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = state;
    // ev.events = EPOLLIN | EPOLLOUT | EPOLLET;

    if (epoll_ctl(efd, EPOLL_CTL_MOD, fd, &ev) < 0)
    {
        perror("stl_epoll_modify_event");
    }
}

static void stl_epoll_del_event(int efd, int fd)
{
    struct epoll_event ev;
    ev.data.fd = fd;

    if (epoll_ctl(efd, EPOLL_CTL_DEL, fd, NULL) < 0)
    {
        perror("stl_epoll_del_event");
    }
}
static int stl_epoll_thread_init(stl_epoll_thread *spt, int i, void *stl_epoll_ctx, char *buf, size_t n)
{
    if (spt)
    {

        stl_epoll *sep = (stl_epoll *)stl_epoll_ctx;
        snprintf(buf, n, "/tmp/thd-%d.domain.socket", i);
        unlink(buf);
        spt->sfd = stl_socket_create_domain(buf);
        spt->id = i;
        spt->efd = epoll_create(1024);
        assert(spt->efd != -1);
        stl_socket_nonblocking(spt->sfd);
        spt->stl_epoll_ctx = stl_epoll_ctx;
        spt->stop = false;
        __sync_fetch_and_add(&sep->num_active_threads, 1);
        return 0;
    }
    return -1;
}

static int stl_epoll_thread_deinit(stl_epoll_thread *spt)
{

    if (spt)
    {
        stl_epoll *sep = (stl_epoll *)spt->stl_epoll_ctx;
        if (spt->sfd != -1)
        {
            close(spt->sfd);
            char ptr[256] = {'\0'};
            char *buf = (char *)ptr;
            snprintf(buf, 256, "/tmp/thd-%d.domain.socket", spt->id);
            unlink(buf);
            close(spt->efd);
            close(spt->efd);
            __sync_fetch_and_sub(&sep->num_active_threads, 1);
            spt->efd = spt->sfd = -1;
            spt->stop = true;
        }
        return 0;
    }
    return -1;
}

int stl_epoll_incr_thread(stl_epoll *sep)
{
    if (sep != NULL)
    {
        stl_epoll_thread *target_thd = NULL;
        if (sep->num_active_threads < sep->num_threads)
        {

            for (size_t i = 0; i < sep->num_threads; i++)
            {
                if (sep->threads[i].stop && sep->efd == -1 && sep->sfd == -1)
                {
                    target_thd = &sep->threads[i];
                    break;
                }
            }

            if (target_thd == NULL)
            {

                sep->threads = realloc(sep->threads, (sep->num_threads + 1) * sizeof(stl_epoll_thread));
                target_thd = &sep->threads[sep->num_threads];
                target_thd->id = sep->num_threads;
            }
            int id = target_thd->id;
            char buf[256] = {'\0'};
            stl_epoll_thread_init(target_thd, id, sep, (char *)&buf, 256);
            fprintf(stdout, "incr work thread id=%d\n", id);
            pthread_create(&target_thd->thd, NULL, stl_epoll_thread_func, target_thd);
            __sync_fetch_and_add(&sep->num_threads, 1);
        }
        return 0;
    }
    return -1;
}
int stl_epoll_deincr_thread(stl_epoll *sep)
{
    if(sep && sep->num_active_threads >0) {
        for(size_t i=sep->num_threads-1;i>=0;i--)
        {
            stl_epoll_thread *spt = &sep->threads[i];
            if(!spt->stop && spt->efd !=-1 && spt->sfd !=-1)
            {
                spt->stop=true;
                break;
            }
        }
        return 0;
    }
    return -1;
}

int stl_epoll_init(stl_epoll *sep, char *addr, int num_threads, size_t max_event_size, stl_epoll_io_func io_func)
{
    if (sep && addr)
    {

        char *found = NULL;

        char *addr_info[2] = {NULL};

        addr_info[0] = strtok(addr, ":");
        addr_info[1] = strtok(NULL, ":");
        if (addr_info[0] == NULL || addr_info[1] == NULL)
        {
            return -1;
        }

        int sfd = stl_socket_create_tcp(addr_info[0], addr_info[1], STL_SOCKET_CONNECT_NONBLOCK);
        int efd = epoll_create(1024);
        if (sfd != -1 && efd != -1)
        {
            sep->efd = efd;
            sep->sfd = sfd;
            stl_socket_nonblocking(sfd);
            stl_socket_set_reuse_adddr(sfd);
            stl_epoll_add_event(sep->efd, sfd);
            sep->io_func = io_func;
            if (num_threads > 0)
            {
                sep->max_event_size = (max_event_size < STL_EPOLL_INIT_EVENT_SIZE) ? STL_EPOLL_INIT_EVENT_SIZE : max_event_size;
                sep->threads = calloc(num_threads, sizeof(stl_epoll_thread));
                sep->num_threads = num_threads;
                char tmp_buf[256] = {'\0'};
                for (size_t i = 0; i < num_threads; i++)
                {
                    stl_epoll_thread_init(&sep->threads[i], i, sep, (char *)&tmp_buf, 256);
                }
            }
        }
        return 0;
    }
    return -1;
}
stl_epoll *stl_epoll_alloc(char *addr, int num_threads, size_t max_event_size, stl_epoll_io_func io_func)
{
    stl_epoll *sep = calloc(1, sizeof(stl_epoll));
    assert(sep != NULL);
    if (stl_epoll_init(sep, addr, num_threads, max_event_size, io_func) != 0)
    {
        free(sep);
        sep = NULL;
    }
    return sep;
}

static void *stl_epoll_thread_func(void *args)
{

    pthread_detach(pthread_self());
    stl_epoll_thread *spt = (stl_epoll_thread *)args;
    stl_epoll *sep = (stl_epoll *)spt->stl_epoll_ctx;
    struct epoll_event *events = calloc(sep->max_event_size, sizeof(*events));
    if (events == NULL)
    {
        perror("calloc(3) failed when attempting to allocate events buffer");
        pthread_exit(NULL);
    }
    int sfd = spt->sfd;
    int efd = spt->efd;
    int events_cnt;
    while ((events_cnt = epoll_wait(efd, events, sep->max_event_size, -1)) > 0)
    {

        for (int i = 0; i < events_cnt; i++)
        {

            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP))
            {
                /* An error has occured on this fd */
                fprintf(stderr, "worker epoll error\n");
                stl_epoll_del_event(spt->efd, events[i].data.fd);
                close(events[i].data.fd);
                continue;
            }

            if (events[i].events & EPOLLIN)
            {
                /* There's some data to be processed */
                if (events[i].data.fd < 0)
                {
                    continue;
                }
                int done = sep->io_func(sep->io_func_ctx, events[i].data.fd);
                if (done != 0)
                {
                    fprintf(stdout, "Closing connection\n");
                    close(events[i].data.fd);
                    stl_epoll_del_event(efd, events[i].data.fd);
                }
            }
            else if (events[i].events & EPOLLOUT)
            {
                // todo
            }
        }
        if (spt->stop)
        {
            fprintf(stdout, "thread idx=%d stop!\n", spt->id);
            break;
        }
    }
    if (spt->stop)
    {
        stl_epoll_thread_deinit(spt);
    }
    free(events);
    pthread_exit(NULL);
    return NULL;
}

static void stl_epoll_handle_accept(stl_epoll *sep, int fd)
{

    if (fd == sep->sfd)
    {
        int sock;
        struct sockaddr addr;
        socklen_t addrlen = sizeof(addr);

        if ((sock = accept(fd, (struct sockaddr *)&addr, &addrlen)) != -1)
        {

            stl_socket_nonblocking(sock);

            size_t index = sock % sep->num_threads;
            stl_epoll_add_event(sep->threads[index].efd, sock);
            fprintf(stdout, "dispath to thread:%ld,client fd=%d\n", index, sock);
        }
    }
}

int stl_epoll_run(stl_epoll *sep)
{

    if (sep == NULL || sep->io_func == NULL)
    {
        return -1;
    }
    int done = 0;
    struct epoll_event *events = calloc(sep->max_event_size, sizeof(*events));
    if (events == NULL)
    {
        perror("calloc(3) failed when attempting to allocate events buffer");
        pthread_exit(NULL);
    }

    for (int i = 0; i < sep->num_threads; ++i)
    {
        pthread_create(&sep->threads[i].thd, NULL, stl_epoll_thread_func, &sep->threads[i]);
    }

    int nfds;
    int sfd = sep->sfd;

    while (1)
    {
        /* Reset the cycle of the round-robin selection of epoll fds */
        if ((nfds = epoll_wait(sep->efd, events, sep->max_event_size, -1)) == -1)
        {
            perror("epoll_wait");
            exit(0);
        }
        for (int i = 0; i < nfds; ++i)
        {
            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP))
            {

                perror("epoll error");
                close(events[i].data.fd);

                continue;
            }
            fprintf(stdout, "sfd=%d,cfd=%d\n", sfd, events[i].data.fd);
            if (events[i].data.fd == sfd)
            {
                stl_epoll_handle_accept(sep, events[i].data.fd);
            }
            else if (events[i].events & EPOLLIN)
            {
            }
            else if (events[i].events & EPOLLOUT)
            {
            }
        }
    }
    free(events);
    return 0;
}
#ifdef TEST
static int stl_epoll_io_cb(void *ctx, int fd)
{
    char buf[4096] = {'\0'};
    read(fd, (char *)&buf, 4096);
    fprintf(stdout, "stl_epoll_read_func:  fd=%d,msg=%s\n", fd, (char *)&buf);
    size_t len = strlen((char *)&buf);
    for (size_t i = 0; i < len; i++)
    {
        if (buf[i] >= 'A' && buf[i] <= 'Z')
        {
            buf[i] += 32;
        }
        if (buf[i] >= 'a' && buf[i] <= 'z')
        {
            buf[i] -= 32;
        }
    }
    write(fd, (char *)&buf, len);
    return 0;
}

static void *mgr_thread(void *arg) {
    stl_epoll *sep = (stl_epoll *)arg;
    size_t i = 0;
    while(1) {
        sleep(3);
        i = rand() %87;
        if(i%2==0) {
         stl_epoll_incr_thread(sep); 

        }else {
            stl_epoll_deincr_thread(sep);
        }


    }
    return NULL;
}
static void *start_epoll(void *arg) {
     stl_epoll *sep = (stl_epoll *)arg;
      stl_epoll_run(sep);
      return NULL;
}
int main()
{
    char addr[64] = {"127.0.0.1:6000"};
    stl_epoll ep;
    pthread_t  id[2];
    stl_epoll_init(&ep, (char *)&addr, 3, 1024, stl_epoll_io_cb);
    stl_epoll_set_context(&ep, NULL);
    pthread_create(&id[0],NULL,start_epoll,&ep);
     pthread_create(&id[1],NULL,mgr_thread,&ep);
    for(size_t i=0;i<2;i++)
    {
        pthread_join(id[0],NULL);
    }
    return 0;
}
#endif