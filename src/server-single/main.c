//
// Created by Nicholas Amor on 6/4/17.
//

#include "server-common.h"
#include "http.h"

char *const server_name = "server-single";

void server_setup() {

}

void server_accept(int socket_fd) {
    ++server_state.total_requests;
    ++server_state.connections;
    http_respond(socket_fd);
    --server_state.connections;
}

void server_shutdown() {

}
