/* */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#include "common.h"

void
dbgprint(char *string)
{
  fprintf(stderr, "%s\n", string);
}

/* config file values */
#define CFG_VAL_ON "ON"
#define CFG_VAL_OFF "OFF"

/* returns 1 if a file named filename exists, 0 otherwise */
int
file_exists(const char * filename)
{
  FILE *file = fopen(filename, "r");
  if (file)
    {
      fclose(file);
      return 1;
    }
  return 0;
}

/* returns file length in bytes, or -1 if no such file exists */
int
file_length(const char *filename)
{
  if (file_exists(filename))
    {
      /* get content length */
      struct stat stbuf;
      stat(filename, &stbuf);

      return (int) stbuf.st_size;
    }
  else
    {
      return -1;
    }
}

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
      if (errno != EAI_AGAIN)
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

/* return the last item in a linked list */
struct list_t *
list_tail(struct list_t * list)
{
  if (list->next == NULL)
    {
      return list;
    }
  return list_tail(list->next);
}

/* wrapper for getaddrinfo */
int
i_getaddrinfo(const char * name, const char * service,
    const struct addrinfo * hints, struct addrinfo ** res)
{
  int igetaddr = getaddrinfo(name, service, hints, res);
  if (igetaddr != 0)
    {
      fprintf(stderr, "%s: %s\n", name, gai_strerror(igetaddr));
      exit(EXIT_FAILURE);
    }

  return igetaddr;
}

/* parse a string (str) as either ON or OFF, return 1 for ON, 0 otherwise */
int
parse_boolstr_value(const char *str)
{
  if (strcmp(str, CFG_VAL_ON) == 0)
    {
      return TRUE;
    }
  else if (strcmp(str, CFG_VAL_OFF))
    {
      return FALSE;
    }
  else
    {
      return -1;
    }
}

/* parse a file referred to by filename to a list of key-value pairs */
struct list_t *
parse_config(const char *filename)
{
  FILE *file;
  char readline[80];
  struct list_t *list = NULL;
  memset(readline, 0, sizeof(char) * 80);

  file = fopen(filename, "r");

  while (fgets(readline, 80, file))
    {
      // skip line if necessary
      if (readline[0] == '#')
        {
          continue;
        }

      // parse line keys and values
      char buff_key[80], buff_val[80];
      sscanf(readline, "%s %s\n", buff_key, buff_val);
      int keylen = strlen(buff_key), valuelen = strlen(buff_val);

      // create key value pairs
      struct key_value_t *parsed_line;
      parsed_line = malloc(sizeof(struct key_value_t));

      parsed_line->key = malloc(sizeof(char) * keylen);
      memcpy(parsed_line->key, buff_key, keylen);

      parsed_line->value = malloc(sizeof(char) * valuelen);
      memcpy(parsed_line->value, buff_val, valuelen);

      // add to list
      struct list_t *new;
      new = malloc(sizeof(struct list_t));
      new->data = parsed_line;
      new->next = NULL;
      if (list)
        {
          struct list_t *tail = list_tail(list);
          tail->next = new;
        }
      else
        {
          list = new;
        }
    }

  fclose(file);

  return list;
}

/* for now, simple string hashing function */
unsigned int
strhash(char *string)
{
  unsigned int hash = 0;
  int i = 0;

  while (i < (int) strlen(string))
    {
      // SDBM hash
      hash = string[i] + (hash << 6) + (hash << 16) - hash;
      ++i;
    }

  return hash;
}

/* read a file to a stream of unsigned chars */
unsigned char *
filegetc(const char *filename)
{
  unsigned char *content = NULL;
  int content_length = file_length(filename);
  if (content_length > 0)
    {
      content = calloc(content_length + 1, sizeof(char));

      FILE *content_file = fopen(filename, "r");
      int bytes_read = 0, readin = 0;
      while (bytes_read < content_length)
        {
          readin = fgetc(content_file);
          if (readin < 0)
            {
              fprintf(stderr, "UNEXPECTED END OF FILE\n");
            }
          else
            {
              content[bytes_read] = readin;
            }
          ++bytes_read;
        }
      fclose(content_file);
    }
  return content;
}

/* returns a freshly allocated string containing the date/time in GMT */
#define DATE_LENGTH 32
char *
strdate(void)
{
  char *date = calloc(DATE_LENGTH, sizeof(char));
  time_t mytime = time(NULL);
  struct tm *mytimet = gmtime(&mytime);
  strftime(date, sizeof(char) * DATE_LENGTH, "%a, %d %b %Y %X GMT", mytimet);
  return date;
}

