/*
 * config-file.c
 *
 *  Created on: Sep 21, 2011
 *      Author: nick
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config-file.h"
#include "common.h"

/* parse a file referred to by filename to a list of key-value pairs */
struct list_t *parse_config(const char *filename) {
    FILE *file = fopen(filename, "r");
    char readline[80];
    struct list_t *list = NULL;
    memset(readline, 0, sizeof(char) * 80);

    while (fgets(readline, 80, file)) {
        // skip line if necessary
        if (readline[0] == '#') {
            continue;
        }

        // parse line keys and values
        char buff_key[80], buff_val[80];
        sscanf(readline, "%s %s\n", buff_key, buff_val);
        size_t keylen = strlen(buff_key), valuelen = strlen(buff_val);

        // create key value pairs
        struct key_value_t *parsed_line;
        parsed_line = malloc(sizeof(struct key_value_t));

        parsed_line->key = calloc(keylen + 1, sizeof(char));
        memcpy(parsed_line->key, buff_key, keylen);

        parsed_line->value = calloc(valuelen + 1, sizeof(char));
        memcpy(parsed_line->value, buff_val, valuelen);

        list_append(&list, parsed_line);
    }

    fclose(file);

    return list;
}

/* parse a string (str) as either ON or OFF, return 1 for ON, 0 otherwise */
int parse_boolstr_value(const char *str) {
    if (strcmp(str, CFG_VAL_ON) == 0) {
        return TRUE;
    } else if (strcmp(str, CFG_VAL_OFF)) {
        return FALSE;
    } else {
        return -1;
    }
}
