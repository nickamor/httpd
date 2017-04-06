//
// Created by Nicholas Amor on 6/4/17.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "list.h"

#ifndef COMMON_FILE_H_
#define COMMON_FILE_H_

#define TRUE 1
#define FALSE 0

struct key_value_t {
    char *key;
    char *value;
};

int file_exists(const char *);

size_t file_length(const char *);

unsigned int strhash(char *);

char *strdate(void);

unsigned char *filegetc(const char *);

char *get_content_type(const char *, struct list_t *list);

#endif
