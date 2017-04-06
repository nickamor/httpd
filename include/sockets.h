/*
 * sockets.h
 *
 *  Created on: Sep 21, 2011
 *      Author: nick
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_

int i_socket(int, int, int);

int i_bind(int, const struct sockaddr *, socklen_t);

int i_listen(int, int);

int i_accept(int, struct sockaddr *, socklen_t *);

ssize_t i_recvfrom(int, void *, size_t, int, struct sockaddr *, socklen_t *);

ssize_t i_sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);

int i_getaddrinfo(const char *, const char *, const struct addrinfo *,
              struct addrinfo **);

#endif /* SOCKETS_H_ */
