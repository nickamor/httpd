#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#ifndef COMMON_FILE_H_
#define COMMON_FILE_H_

#define TRUE 1
#define FALSE 0

struct server_state_t
{
  int accepting;
  int connections;
};

struct server_config_t
{
  char * server_root;
};

struct server_state_t server_state;

struct server_config_t server_config;

const char *default_root;

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

struct list_t *
list_tail(struct list_t*);

int
i_getaddrinfo(const char *, const char *, const struct addrinfo *,
    struct addrinfo **);

unsigned int
strhash(char *);

char *
strdate(void);

unsigned char *
filegetc(const char *);

char *
get_content_type(const char *);

void
dbgprint(char *);

#endif
