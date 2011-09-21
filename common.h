#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#ifndef common_h

#define TRUE 1
#define FALSE 0

void
dbgprint(char *);

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
file_length(const char *);

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

char *
strdate(void);

unsigned char *
filegetc(const char *);

#define common_h
#endif
