//
// Created by Nicholas Amor on 6/4/17.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#include "common.h"
#include "server-common.h"
#include "sockets.h"

void stop_accepting() {
    printf("\nExiting...\n");
    server_state.accepting = FALSE;
}

int main(int argc, char **argv) {
    /* test arguments */
    if (argc < 2) {
        fprintf(stderr, "usage: %s settings.config\n", server_name);
        exit(0);
    }

    /* handle config */
    if (!read_config(argv[1])) {
        return EXIT_FAILURE;
    }

    log_append("%s initialisation complete", server_name);

    /* create socket */
    server_state.listen_socket = i_socket(PF_INET, SOCK_STREAM, 0);
    // set the socket to be reusable
    int on = 1;
    setsockopt(server_state.listen_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &on,
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
    quit_action.sa_flags = SA_NOCLDSTOP;
    sigaction(server_config.shutdown_signal, &quit_action, NULL);

    server_setup();

    /* remember parent pid */
    server_state.parent_pid = getpid();

    log_append("Listening for connections on port %d", server_config.port);

    while (server_state.accepting) {
        /* accept new connections */
        struct sockaddr_in cliaddr;
        memset(&cliaddr, 0, sizeof(cliaddr));
        socklen_t cliaddrlen = sizeof(cliaddr);
        int clisock = i_accept(server_state.listen_socket,
                               (struct sockaddr *) &cliaddr, &cliaddrlen);
        if (clisock > 0) {
            server_accept(clisock);
        }
    }

    close(server_state.listen_socket);
    server_shutdown();

    log_append("All connections closed - Terminating server");

    return EXIT_SUCCESS;
}
