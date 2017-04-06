//
// Created by Nicholas Amor on 6/4/17.
//

#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <stdio.h>

#include "server-common.h"
#include "http.h"

char *const server_name = "server-threaded";

struct thread_info_t {
    int socket_fd;
};

void thread_routine(void *thread_info) {
    struct thread_info_t *info = thread_info;

    http_respond(info->socket_fd);

    pthread_detach(pthread_self());
}

void server_setup() {

}

void server_accept(int socket_fd) {
    ++server_state.total_requests;

    ++server_state.connections;

    pthread_t new_thread;
    pthread_attr_t new_thread_attr;
    pthread_attr_init(&new_thread_attr);

    struct thread_info_t thread_info = { socket_fd };

    int creation_error = pthread_create(&new_thread, &new_thread_attr, (void *)&thread_routine, (void *) &thread_info);

    if (creation_error != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    --server_state.connections;
}

void server_shutdown() {

}
