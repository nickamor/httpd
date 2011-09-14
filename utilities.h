/*
 * utilities.h
 *
 *  Created on: Aug 9, 2011
 *      Author: Nick Amor, s3240873
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#ifndef UTILITIES_H_
#define UTILITIES_H_

struct list_t
{
  void *data;
  struct list_t *next;
};

struct key_value_t
{
  char *key;
  char *value;
};

int
file_exists(const char *);

int
i_socket(int, int, int);

int
i_bind(int, const struct sockaddr *, socklen_t);

int
i_listen(int, int);

int
i_accept(int, struct sockaddr *, socklen_t *);

ssize_t
i_recvfrom(int, void *, size_t, int, struct sockaddr *, socklen_t *);

ssize_t
i_sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);

struct list_t *
list_tail(struct list_t*);

int
i_getaddrinfo(const char *, const char *, const struct addrinfo *,
    struct addrinfo **);

int
parse_boolstr_value(const char *);

struct list_t *
parse_config(const char *);

unsigned int
strhash(char *);

#endif /* UTILITIES_H_ */
