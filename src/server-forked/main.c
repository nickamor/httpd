//
// Created by Nicholas Amor on 6/4/17.
//

#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "server-forked/childpid.h"
#include "server-common.h"
#include "http.h"

const char *server_name = "server-forked";

void remove_pid() {
    pid_t childpid = wait(NULL);

    remove_childpid(childpid);
}

void server_setup() {
    /* register child quit handler */
    struct sigaction child_finished;
    child_finished.sa_handler = (void *) remove_pid;
    sigemptyset(&child_finished.sa_mask);
    child_finished.sa_flags = 0;
    sigaction(SIGCHLD, &child_finished, NULL);

}

void server_accept(int socket_fd) {
    ++server_state.total_requests;
    pid_t childpid = fork();
    if (childpid == 0) {
        close(server_state.listen_socket);

        ++server_state.connections;

        http_respond(socket_fd);

        --server_state.connections;

        exit(0);
    } else {
        add_childpid(childpid);

        close(socket_fd);
    }
}

void server_shutdown() {
    /* wait for any loose connections to close */
    while (childpid_list) {
        waitpid(childpid_list->pid, NULL, 0);
    }
}
