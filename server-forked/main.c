
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

#include "childpid.h"
#include "../common.h"
#include "../server-common.h"
#include "../sockets.h"
#include "../http.h"
#include "../config-file.h"

const char * server_name = "server-forked";

int
main(int, char **);

void
stop_accepting(void);

void
stop_accepting()
{
  server_state.accepting = FALSE;
}

struct childpid_list_t
{
  pid_t pid;
  struct childpid_list_t * next;
};

struct childpid_list_t * childpid_list = NULL;

void
add_childpid(pid_t childpid)
{
  /* add the new child to the child manager */
  struct childpid_list_t * new_childpid = calloc(1,
      sizeof(struct childpid_list_t));
  new_childpid->pid = childpid;
  new_childpid->next = NULL;

  if (childpid_list)
    {
      struct childpid_list_t * tail_childpid = childpid_list;
      while (tail_childpid->next)
        {
          tail_childpid = tail_childpid->next;
        }
      tail_childpid->next = new_childpid;
    }
  else
    {
      childpid_list = new_childpid;
    }
}

void
remove_childpid(pid_t childpid)
{
  /* find the child to remove */
  struct childpid_list_t * childpid_to_remove = NULL, *prev_childpid = NULL;

  childpid_to_remove = childpid_list;
  while (childpid_to_remove && childpid_to_remove->pid != childpid)
    {
      prev_childpid = childpid_to_remove;
      childpid_to_remove = childpid_to_remove->next;
    }

  /* remove the child from the child manager */
  if (childpid_to_remove)
    {
      struct childpid_list_t * next_childpid = childpid_to_remove->next;

      if (prev_childpid)
        {
          prev_childpid->next = next_childpid;
        }

      if (childpid_list == childpid_to_remove)
        {
          childpid_list = childpid_to_remove->next;
        }

      free(childpid_to_remove);
    }
}

void
child_exits()
{
  pid_t childpid = wait(NULL);

  remove_childpid(childpid);
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
  // set the socket to be reusable
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
  struct sigaction quit_action;
  quit_action.sa_handler = (void *) stop_accepting;
  sigemptyset(&quit_action.sa_mask);
  quit_action.sa_flags = SA_NOCLDSTOP;
  sigaction(server_config.shutdown_signal, &quit_action, NULL);

  /* register child quit handler */
  struct sigaction child_finished;
  child_finished.sa_handler = (void *) child_exits;
  sigemptyset(&child_finished.sa_mask);
  child_finished.sa_flags = 0;
  sigaction(SIGCHLD, &child_finished, NULL);

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
          pid_t childpid = fork();
          if (childpid == 0)
            {
              close(server_state.listen_socket);

              ++server_state.connections;

              http_respond(clisock);

              --server_state.connections;

              exit(0);
            }
          else
            {
              add_childpid(childpid);

              close(clisock);
            }
        }
    }

  close(server_state.listen_socket);

  /* wait for any loose connections to close */
  while (childpid_list)
    {
      waitpid(childpid_list->pid, NULL, 0);
    }

  log_write(time(NULL), "all connections closed");
  log_write(time(NULL), "terminating server");

  return EXIT_SUCCESS;
}
