/*
 * sockets.c
 *
 *  Created on: Sep 21, 2011
 *      Author: nick
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/* wrapper for socket() */
int
i_socket(int domain, int type, int protocol)
{
  int socket_fd = socket(domain, type, protocol);
  if (socket_fd == -1)
    {
      perror("socket");
      exit(EXIT_FAILURE);
    }
  return socket_fd;
}

/* wrapper for bind() */
int
i_bind(int sockfd, const struct sockaddr * addr, socklen_t addrlen)
{
  int ibind = bind(sockfd, addr, addrlen);
  if (ibind == -1)
    {
      perror("bind");
      exit(EXIT_FAILURE);
    }
  return ibind;
}

/* wrapper for listen() */
int
i_listen(int sockfd, int backlog)
{
  int ilisten = listen(sockfd, backlog);
  if (ilisten == -1)
    {
      perror("listen");
      exit(EXIT_FAILURE);
    }
  return ilisten;
}

/* wrapper for accept() */
int
i_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
  errno = 0;
  int iaccept = accept(sockfd, addr, addrlen);
  if (iaccept == -1)
    {
      if (errno != EINTR)
        {
          perror("accept");
          exit(EXIT_FAILURE);
        }
    }
  return iaccept;
}

/* wrapper for recvfrom() */
ssize_t
i_recvfrom(int sockfd, void * buf, size_t len, int flags,
    struct sockaddr * src_addr, socklen_t * addrlen)
{
  ssize_t irecvfrom = recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
  if (irecvfrom == -1)
    {
      perror("recvfrom");
      exit(EXIT_FAILURE);
    }
  return irecvfrom;
}

/* wrapper for sendto() */
ssize_t
i_sendto(int sockfd, const void * buf, size_t len, int flags,
    const struct sockaddr * dest_addr, socklen_t addrlen)
{
  ssize_t isendto = sendto(sockfd, buf, len, flags, dest_addr, addrlen);
  if (isendto == -1)
    {
      perror("sendto");
      exit(EXIT_FAILURE);
    }
  return isendto;
}
