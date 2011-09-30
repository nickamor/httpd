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
  int listen_socket;
  int total_requests;
  int parent_pid;
};

struct server_config_t
{
  int port;
  char * root;
  char * host;

  int shutdown_signal;
  char * shutdown_request;
  char * status_request;

  char * logfile;
  int logging;
  char * recordfile;
  int recording;

  struct list_t * mime_types;

  char * name;
};

struct server_state_t server_state;

struct server_config_t server_config;

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

unsigned int
strhash(char *);

char *
strdate(void);

unsigned char *
filegetc(const char *);

char *
get_content_type(const char *, struct list_t * list);

void
dbgprint(char *);

#endif
