//
// Created by Nicholas Amor on 6/4/17.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "common.h"
#include "list.h"

/* returns 1 if a file named filename exists, 0 otherwise */
int file_exists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return TRUE;
    }
    return FALSE;
}

/* returns file length in bytes, or -1 if no such file exists */
size_t file_length(const char *filename) {
    if (file_exists(filename)) {
        /* get content length */
        struct stat stbuf;
        stat(filename, &stbuf);

        return (int) stbuf.st_size;
    } else {
        return -1;
    }
}

/* for now, simple string hashing function */
unsigned int strhash(char *string) {
    unsigned int hash = 0;
    int i = 0;

    while (i < (int) strlen(string)) {
        // SDBM hash
        hash = string[i] + (hash << 6) + (hash << 16) - hash;
        ++i;
    }

    return hash;
}

/* read a file to a stream of unsigned chars */
unsigned char *filegetc(const char *filename) {
    unsigned char *content = NULL;
    size_t content_length = file_length(filename);
    if (content_length > 0) {
        content = calloc(content_length + 1, sizeof(char));

        FILE *content_file = fopen(filename, "r");
        if (content_file == NULL) {
            fprintf(stderr, "Error opening file %s\n", filename);
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        int bytes_read = 0, readin = 0;
        while (bytes_read < content_length) {
            readin = fgetc(content_file);
            if (readin < 0) {
                fprintf(stderr, "Unexpected end of file reading %s\n", filename);
                perror("fgetc");
                exit(EXIT_FAILURE);
            } else {
                content[bytes_read] = (unsigned char)readin;
            }
            ++bytes_read;
        }
        fclose(content_file);
    }
    return content;
}

/* returns a freshly allocated string containing the date/time in GMT */
#define DATE_LENGTH 32

char *strdate(void) {
    char *date = calloc(DATE_LENGTH, sizeof(char));
    time_t mytime = time(NULL);
    struct tm *mytimet = gmtime(&mytime);
    strftime(date, sizeof(char) * DATE_LENGTH, "%a, %d %b %Y %X GMT", mytimet);
    return date;
}

char *get_content_type(const char *filename, struct list_t *mime_types) {
    struct list_t *iter = mime_types;

    char *filetype = strrchr(filename, '.') + 1;

    while (iter) {
        struct key_value_t *keyval = iter->data;

        if (!strchr(keyval->key, '-')) {
            return keyval->value;
        }

        char *itertype = strrchr(keyval->key, '-') + 1;

        if (strcmp(itertype, filetype) == 0) {
            return keyval->value;
        }

        iter = iter->next;
    }

    return NULL;
}

void resize_buffer(void **buffer, size_t *size, size_t new_size) {
    char *new_buffer = (char *) realloc(*buffer, new_size);
    if (new_buffer == NULL) {
        fprintf(stderr, "Failed reallocating buffer");
        perror("realloc");
        exit(EXIT_FAILURE);
    }
    *buffer = new_buffer;
    *size = new_size;
}
