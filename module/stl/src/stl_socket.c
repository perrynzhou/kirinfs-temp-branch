/*************************************************************************
    > File Name: stl_socket.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Wed 07 Sep 2022 04:34:52 AM UTC
 ************************************************************************/

#include "stl_socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <arpa/inet.h>

static int stl_socket_set_blocking(int fd, int non_block);
int stl_socket_create_tcp(const char *host, const char *port, int flags)
{
   struct sockaddr_in addr;

    int sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    /* retrieve the port number for listening */

    /* setup the address structure */
    /* use INADDR_ANY to bind to all local addresses  */
    bzero(&addr, sizeof(addr)); 
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(port));


    bind(sfd,(struct sockaddr *)&addr,sizeof(addr));
    listen(sfd, 50000);

     return sfd;


}

int stl_socket_create_domain(const char *socket_path)
{

  struct sockaddr_un srv_addr;
  socklen_t cliun_len;
  int sfd, connfd, size;
  int i, n;

  if ((sfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
  {
    perror("socket error");
    return -1;
  }

  memset(&srv_addr, 0, sizeof(srv_addr));
  srv_addr.sun_family = AF_UNIX;
  strncpy(srv_addr.sun_path, socket_path, strlen(socket_path));

  if (bind(sfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0)
  {
    perror("bind error");
    close(sfd);
    return -1;
  }

  return sfd;
}
static int stl_socket_set_blocking(int fd, int non_block)
{
  int flags;

  if ((flags = fcntl(fd, F_GETFL)) == -1)
  {
    fprintf(stderr, "fcntl(F_GETFL): %s", strerror(errno));
    return -1;
  }
  if (!!(flags & O_NONBLOCK) == !!non_block)
    return 0;

  if (non_block)
    flags |= O_NONBLOCK;
  else
    flags &= ~O_NONBLOCK;

  if (fcntl(fd, F_SETFL, flags) == -1)
  {
    fprintf(stderr, "fcntl(F_SETFL,O_NONBLOCK): %s", strerror(errno));
    return -1;
  }
  return 0;
}

int stl_socket_set_reuse_adddr(int fd)
{
  if (fd != -1)
  {
    int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
    {
      fprintf(stderr, "setsockopt SO_REUSEADDR: %s", strerror(errno));
      return -1;
    }
    return 0;
  }
  return -1;
}

int stl_socket_send(int fd, char *buf, size_t *len)
{

  int total = 0;
  size_t left = *len;
  int n;

  while (total < *len)
  {
    n = write(fd, buf + total, left);
    if (n == -1)
      break;
    total += n;
    left -= n;
  }

  *len = total;

  return n == -1 ? -1 : 0;
}

int stl_socket_recv(int fd, char *buf, size_t *len)
{
  int total = 0;
  size_t left = *len;
  int n;

  while (total < *len)
  {
    n = read(fd, buf + total, left);
    if (n == -1)
      break;
    total += n;
    left -= n;
  }

  *len = total;

  return n == -1 ? -1 : 0;
}

int stl_socket_nonblocking(int fd)
{

      int flags, result;
    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return -1;
    }
    result = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    if (result == -1) {
        perror("fcntl");
        return -1;
    }
    return 0;
  // return stl_socket_set_blocking(fd, 1);
   
}

int stl_socket_blocking(int fd)
{
  return stl_socket_set_blocking(fd, 0);
}

#ifdef TEST
#include <unistd.h>
#include <stdlib.h>
int main()
{
  int sfd = stl_socket_create_tcp("127.0.0.1", "9090", 1);
  fprintf(stdout, "sfd=%d\n", sfd);
      int sock;
    struct sockaddr addr;
    socklen_t addrlen = sizeof(addr);

  
     
  while(1) {
    if ((sock = accept(sfd, (struct sockaddr *)&addr, &addrlen)) != -1)
    {
        fprintf(stdout,"sfd=%d,cfd=%d\n",sfd,sock);
    }
  }
 
}
#endif