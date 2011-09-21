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
  if (argc == 2 || TRUE) {
      char ** args = argv;
      args++;
  }

  /* handle config */
  // TODO: implement server_config options
  // TODO: read server_config options from file
  server_config.port = 40000;
  server_config.root = "../httpdoc";
  server_config.host = "goanna.cs.rmit.edu.au";
  server_config.shutdown_signal = 15;
  server_config.shutdown_request = "config/shutdown.htm";
  server_config.status_request = "config/status.htm";
  server_config.logfile = "webserver.log";
  server_config.logging = TRUE;
  server_config.recordfile = "lastrequest.txt";
  server_config.recording = TRUE;
  struct key_value_t typetxt = {"type-txt", "text/plain"};
  struct key_value_t typehtm = {"type-htm", "text/html"};
  struct key_value_t typehtml = {"type-html", "text/html"};
  struct key_value_t typejpg = {"type-jpg", "image/jpeg"};
  struct key_value_t typemp3 = {"type-mp3", "audio/mpeg"};
  struct key_value_t typewav = {"type-wav", "audio/vnd.wave"};
  struct key_value_t typenone = {"type", "text/plain"};
  struct list_t typelist7 = {&typenone, NULL};
  struct list_t typelist6 = {&typewav, &typelist7};
  struct list_t typelist5 = {&typemp3, &typelist6};
  struct list_t typelist4 = {&typejpg, &typelist5};
  struct list_t typelist3 = {&typehtml, &typelist4};
  struct list_t typelist2 = {&typehtm, &typelist3};
  struct list_t typelist = {&typetxt, &typelist2};
  server_config.mime_types = &typelist;

  server_config.name = "server-single";

  /* create socket */
  int listen_socket = i_socket(PF_INET, SOCK_STREAM, 0);
  // set the socket to be reusable (helpful when testing)
  int on = 1;
  setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &on, sizeof(on));

  /* bind socket to listening port and set to listen*/
  struct sockaddr_in myaddr;
  myaddr.sin_family = PF_INET;
  myaddr.sin_port = htons(server_config.port);
  myaddr.sin_addr.s_addr = INADDR_ANY;
  i_bind(listen_socket, (struct sockaddr *) &myaddr, sizeof(myaddr));
  i_listen(listen_socket, 5);


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
