#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "../common.h"
#include "../sockets.h"
#include "../http.h"

/* server state */
struct server_state_t server_state =
  { TRUE, 0 };

/* server settings */
struct server_config_t server_config;

int
main(void);

void
stop_accepting(void);

void
stop_accepting()
{
  printf("\nExiting...\n");
  server_state.accepting = FALSE;
}

int
main()
{
  int listen_socket = i_socket(PF_INET, SOCK_STREAM, 0);

  /* set the socket to be reusable */
  int on = 1;
  setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &on, sizeof(on));

  /* bind socket to listening port and set to listen*/
  struct sockaddr_in myaddr;
  myaddr.sin_family = PF_INET;
  myaddr.sin_port = htons(40000);
  myaddr.sin_addr.s_addr = INADDR_ANY;
  i_bind(listen_socket, (struct sockaddr *) &myaddr, sizeof(myaddr));
  i_listen(listen_socket, 5);

  /* handle config */
  server_config.server_root = (char *) default_root;

  /* register interrupt handler */
  struct sigaction new_action;
  new_action.sa_handler = (void *) stop_accepting;
  sigemptyset(&new_action.sa_mask);
  new_action.sa_flags = 0;
  int signal_to_listen_for = SIGTERM;
  sigaction(signal_to_listen_for, &new_action, NULL);

  while (server_state.accepting)
    {
      /* accept new connections */
      struct sockaddr_in cliaddr;
      memset(&cliaddr, 0, sizeof(cliaddr));
      socklen_t cliaddrlen = sizeof(cliaddr);
      int clisock = i_accept(listen_socket, (struct sockaddr *) &cliaddr,
          &cliaddrlen);
      if (clisock > 0)
        {
          ++server_state.connections;
          http_respond(clisock);
          --server_state.connections;
        }
    }

  close(listen_socket);

  while (server_state.connections > 0)
    ;

  return EXIT_SUCCESS;
}
