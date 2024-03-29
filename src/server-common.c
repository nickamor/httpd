/*
 * server-common.c
 *
 *  Created on: Oct 3, 2011
 *      Author: nick
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "common.h"
#include "server-common.h"
#include "config-file.h"
#include "list.h"

/* server state */
struct server_state_t server_state =
        {TRUE, 0, 0, 0, 0};

/* server settings */
struct server_config_t server_config =
        {
                // Default port
                80,

                // Default document root to serve
                "./",

                // Default host
                "localhost",

                // Signal ID to register shutdown action
                9999,

                // HTTP request to this method will trigger shutdown
                "shutdown",

                // HTTP request to this method will return server status message
                "status",

                // Default server logging file
                "httpd.log",

                // Enable logging
                TRUE,

                // Default server access logging file
                "requests.log",

                // Enable access logging
                FALSE,

                // Mime type lookup list
                NULL,

                // Server name
                "Default server name"
        };

int read_config(const char *filename) {
    if (!file_exists(filename)) {
        fprintf(stderr, "Could not open %s for reading.\n", filename);
        return FALSE;
    }

    struct list_t *config_keys = parse_config(filename);

    struct list_t *iter = config_keys;
    while (iter) {
        struct key_value_t *keyval = iter->data;

        if (strcmp(keyval->key, "port") == 0) {
            server_config.port = (int)strtol(keyval->value, NULL, 0);
        } else if (strcmp(keyval->key, "root") == 0) {
            server_config.root = keyval->value;
        } else if (strcmp(keyval->key, "host") == 0) {
            server_config.host = keyval->value;
        } else if (strcmp(keyval->key, "shutdown-signal") == 0) {
            server_config.shutdown_signal = (int)strtol(keyval->value, NULL, 0);
        } else if (strcmp(keyval->key, "shutdown-request") == 0) {
            server_config.shutdown_request = keyval->value;
        } else if (strcmp(keyval->key, "status-request") == 0) {
            server_config.status_request = keyval->value;
        } else if (strcmp(keyval->key, "logfile") == 0) {
            server_config.logfile = keyval->value;
        } else if (strcmp(keyval->key, "logging") == 0) {
            server_config.logging = ((strcmp(keyval->value, "yes") == 0) ? TRUE : FALSE);
        } else if (strcmp(keyval->key, "recordfile") == 0) {
            server_config.recordfile = keyval->value;
        } else if (strcmp(keyval->key, "recording") == 0) {
            server_config.recording = ((strcmp(keyval->value, "yes") == 0) ? TRUE : FALSE);
        } else if (strstr(keyval->key, "type")) {
            list_append(&server_config.mime_types, keyval->value);
        }

        iter = iter->next;
    }

    server_config.name = (char *) server_name;

    return TRUE;
}

void log_append(const char *format, ...) {
    if (server_config.logging == FALSE) {
        return;
    }

    FILE *logfile = fopen(server_config.logfile, "a");

    if (!logfile) {
        fprintf(stderr, "Could not open %s for writing.\n",
                server_config.logfile);
        return;
    }

    va_list argptr;

    time_t time_now = time(NULL);
    struct tm *tm_now = localtime(&time_now);

    fprintf(logfile, "%04d/%02d/%02dT%02d:%02d:%02d ",
            tm_now->tm_year + 1900,
            tm_now->tm_mon,
            tm_now->tm_mday,
            tm_now->tm_hour,
            tm_now->tm_min,
            tm_now->tm_sec);

    va_start(argptr, format);
    vfprintf(logfile, format, argptr);
    va_end(argptr);

    fprintf(logfile, "\n");

    fclose(logfile);
}
