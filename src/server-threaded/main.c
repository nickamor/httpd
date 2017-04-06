//
// Created by Nicholas Amor on 6/4/17.
//

#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#include "server-common.h"
#include "http.h"

const char *server_name = "server-threaded";

void thread_routine(void *socket_fd) {
    log_append("Thread created");

    http_respond(*(int *)socket_fd);
}

void server_setup() {

}

void server_accept(int socket_fd) {
    ++server_state.total_requests;

    ++server_state.connections;

    pthread_t new_thread;
    pthread_attr_t new_thread_attr;
    pthread_create(&new_thread, &new_thread_attr, (void *)&thread_routine, (void *) &socket_fd);
    pthread_detach(new_thread);

    --server_state.connections;
}

void server_shutdown() {

}
