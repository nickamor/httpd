/* */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "common.h"

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

void
dbgprint(char *string)
{
  fprintf(stderr, "%s\n", string);
}

char *
get_content_type(const char *filename, struct list_t * mime_types)
{
  struct list_t * iter = mime_types;

  char * filetype = strrchr(filename, '.') + 1;

  while (iter)
    {
      struct key_value_t * keyval = iter->data;

      if (!strchr(keyval->key, '-'))
        {
          return keyval->value;
        }

      char * itertype = strrchr(keyval->key, '-') + 1;

      if (strcmp(itertype, filetype) == 0)
        {
          return keyval->value;
        }

      iter = iter->next;
    }

  return NULL;
}

