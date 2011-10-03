/*
 * server-common.c
 *
 *  Created on: Oct 3, 2011
 *      Author: nick
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "server-common.h"
#include "config-file.h"

/* server state */
struct server_state_t server_state =
  { TRUE, 0, 0, 0, 0 };

/* server settings */
struct server_config_t server_config;

int
read_config(const char * filename)
{
  if (!file_exists(filename))
    {
      fprintf(stderr, "Could not open %s for reading.\n", filename);
      return FALSE;
    }

  struct list_t * config_keys = parse_config(filename);

  struct list_t * iter = config_keys;
  while (iter)
    {
      struct key_value_t *keyval = iter->data;

      if (strcmp(keyval->key, "port") == 0)
        {
          server_config.port = strtol(keyval->value, NULL, 0);
        }
      else if (strcmp(keyval->key, "root") == 0)
        {
          server_config.root = keyval->value;
        }
      else if (strcmp(keyval->key, "host") == 0)
        {
          server_config.host = keyval->value;
        }
      else if (strcmp(keyval->key, "shutdown-signal") == 0)
        {
          server_config.shutdown_signal = strtol(keyval->value, NULL, 0);
        }
      else if (strcmp(keyval->key, "shutdown-request") == 0)
        {
          server_config.shutdown_request = keyval->value;
        }
      else if (strcmp(keyval->key, "status-request") == 0)
        {
          server_config.status_request = keyval->value;
        }
      else if (strcmp(keyval->key, "logfile") == 0)
        {
          server_config.logfile = keyval->value;
        }
      else if (strcmp(keyval->key, "logging") == 0)
        {
          server_config.logging = (
              (strcmp(keyval->value, "yes") == 0) ? TRUE : FALSE);
        }
      else if (strcmp(keyval->key, "recordfile") == 0)
        {
          server_config.recordfile = keyval->value;
        }
      else if (strcmp(keyval->key, "recording") == 0)
        {
          server_config.recording = (
              (strcmp(keyval->value, "yes") == 0) ? TRUE : FALSE);
        }
      else if (strstr(keyval->key, "type"))
        {
          struct list_t * new = malloc(sizeof(struct list_t));
          new->data = keyval;
          new->next = NULL;

          if (server_config.mime_types)
            {
              list_tail(server_config.mime_types)->next = new;
            }
          else
            {
              server_config.mime_types = new;
            }
        }

      iter = iter->next;
    }

  server_config.name = (char *) server_name;

  return TRUE;
}
