/*
 * server-common.h
 *
 *  Created on: Oct 3, 2011
 *      Author: nick
 */

#ifndef SERVER_COMMON_H_
#define SERVER_COMMON_H_

const char * server_name;

struct server_state_t
{
  int accepting;
  int connections;
  int listen_socket;
  int total_requests;
  int parent_pid;
};

struct server_config_t
{
  int port;
  char * root;
  char * host;

  int shutdown_signal;
  char * shutdown_request;
  char * status_request;

  char * logfile;
  int logging;
  char * recordfile;
  int recording;

  struct list_t * mime_types;

  char * name;
};

struct server_state_t server_state;

struct server_config_t server_config;

int
read_config(const char * filename);

#endif /* SERVER_COMMON_H_ */
