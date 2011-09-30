#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "../common.h"
#include "../sockets.h"
#include "../http.h"
#include "../config-file.h"

const char * server_name = "server-single";

/* server state */
struct server_state_t server_state =
  { TRUE, 0, 0, 0, 0 };

/* server settings */
struct server_config_t server_config;

int
main(int, char **);

void
stop_accepting(void);

int
read_config(const char * filename);

int
read_config(const char * filename)
{
  FILE * configfile_fp = fopen(filename, "r");

  if (!configfile_fp)
    {
      fprintf(stderr, "Could not open %s for reading.\n", filename);
      return FALSE;
    }

  fclose(configfile_fp);

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

void
stop_accepting()
{
  printf("\nExiting...\n");
  server_state.accepting = FALSE;
}

int
main(int argc, char ** argv)
{
  /* test arguments */
  if (argc < 2)
    {
      fprintf(stderr, "usage: %s settings.config\n", server_name);
    }

  /* handle config */
  if (!read_config(argv[1]))
    {
      return EXIT_FAILURE;
    }

  /* create socket */
  server_state.listen_socket = i_socket(PF_INET, SOCK_STREAM, 0);
  // set the socket to be reusable (helpful when testing)
  int on = 1;
  setsockopt(server_state.listen_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &on,
      sizeof(on));

  /* bind socket to listening port and set to listen*/
  struct sockaddr_in myaddr;
  myaddr.sin_family = PF_INET;
  myaddr.sin_port = htons(server_config.port);
  myaddr.sin_addr.s_addr = INADDR_ANY;
  i_bind(server_state.listen_socket, (struct sockaddr *) &myaddr,
      sizeof(myaddr));
  i_listen(server_state.listen_socket, 5);

  /* register interrupt handler */
  struct sigaction new_action;
  new_action.sa_handler = (void *) stop_accepting;
  sigemptyset(&new_action.sa_mask);
  new_action.sa_flags = 0;
  int signal_to_listen_for = SIGTERM;
  sigaction(signal_to_listen_for, &new_action, NULL);

  /* remember parent pid */
  server_state.parent_pid = getpid();

  while (server_state.accepting)
    {
      /* accept new connections */
      struct sockaddr_in cliaddr;
      memset(&cliaddr, 0, sizeof(cliaddr));
      socklen_t cliaddrlen = sizeof(cliaddr);
      int clisock = i_accept(server_state.listen_socket,
          (struct sockaddr *) &cliaddr, &cliaddrlen);
      if (clisock > 0)
        {
          ++server_state.total_requests;
          ++server_state.connections;
          http_respond(clisock);
          --server_state.connections;
        }
    }

  close(server_state.listen_socket);

  /* wait for any loose connections to close */
  while (server_state.connections > 0)
    ;

  return EXIT_SUCCESS;
}
