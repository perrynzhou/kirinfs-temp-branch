/*************************************************************************
    > File Name: stl_socket.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Wed 07 Sep 2022 04:34:48 AM UTC
 ************************************************************************/

#ifndef _STL_SOCKET_H
#define _STL_SOCKET_H
#include <stdio.h>
#define STL_SOCKET_CONNECT_NONBLOCK 1
#define STL_SOCKET_CONNECT_BLOCK 0
int stl_socket_create_tcp(const char *host, const char *port,int flags);
int stl_socket_create_domain(const char *socket_path);
// int send_all(int sfd, char *buf, int *len)
int stl_socket_send(int fd, char *buf, size_t *len);

int stl_socket_recv(int fd, char *buf, size_t *len);

int stl_socket_nonblocking(int fd);
int stl_socket_blocking(int fd);
int stl_socket_set_reuse_adddr(int fd);
#endif