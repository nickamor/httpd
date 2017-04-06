
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#include "common.h"
#include "server-common.h"
#include "sockets.h"
#include "http.h"
#include "config-file.h"

const char * server_name = "server-single";

int
main(int, char **);

void
stop_accepting(void);

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
      exit(0);
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

  /* bind socket to listening port and set to listen */
  struct sockaddr_in myaddr;
  myaddr.sin_family = PF_INET;
  myaddr.sin_port = htons(server_config.port);
  myaddr.sin_addr.s_addr = INADDR_ANY;
  i_bind(server_state.listen_socket, (struct sockaddr *) &myaddr,
      sizeof(myaddr));
  i_listen(server_state.listen_socket, 5);

  /* register interrupt handler */
  struct sigaction quit_action;
  quit_action.sa_handler = (void *) stop_accepting;
  sigemptyset(&quit_action.sa_mask);
  quit_action.sa_flags = 0;
  sigaction(server_config.shutdown_signal, &quit_action, NULL);

  /* remember parent pid */
  server_state.parent_pid = getpid();

  log_write(time(NULL), "Listening for connections on port %d", myaddr.sin_port);

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
  log_write(time(NULL), "all connections closed");
  log_write(time(NULL), "terminating server");

  return EXIT_SUCCESS;
}