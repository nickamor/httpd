//
// Created by Nicholas Amor on 6/4/17.
//

#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#include "server-common.h"
#include "http.h"

const char *server_name = "server-threaded";

void server_setup() {

}

void server_accept(int socket_fd) {
    ++server_state.total_requests;

    ++server_state.connections;

    pthread_t new_thread;
    pthread_attr_t new_thread_attr;
    pthread_create(&new_thread, &new_thread_attr, (void *) &http_respond, (void *) &socket_fd);
    pthread_detach(new_thread);

    --server_state.connections;
}

void server_shutdown() {

}
