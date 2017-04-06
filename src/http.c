/*
 * http.c
 *
 *  Created on: Sep 21, 2011
 *      Author: nick
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common.h"
#include "server-common.h"
#include "http.h"

#define REQUEST_SIZE 1024
#define RESPONSE_SIZE 1024

const char * doc404_format = "<html><head><title>File Not Found</title></head>"
    "<body><h3>File Not Found</h3><p>The requested document %s was not found"
    " on this server.";

const char * docshutdown =
    "<html><head><title>Server Shutting Down...</title></head>"
        "<body><h3>Server Shutting Down...";

const char * docstatus_format =
    "<html><head><title>Server Status</title></head><body>"
        "<h3>Server Status</h3>"
        "<p id=\"server-name\">%s</p>"
        "<p id=\"date\">Status at %s</p>"
        "<br>"
        "<p id=\"connections\">Active connections: %d</p>"
        "<p id=\"requests\">Total requests: %d</p>"
        "<p id=\"listen-port\">Listening port: %d</p>"
        "<br>"
        "<p id=\"shutdown\">To shutdown, do \"kill -%d %d\" or click "
        "<a href=\"/%s\">here</a>.";

void
http_respond(int clisock)
{
  /* receive bytes */
  int irecv = 0;
  char request[REQUEST_SIZE];
  memset(&request, 0, REQUEST_SIZE);
  irecv = recv(clisock, &request, REQUEST_SIZE, 0);
  int recvtotal = irecv;
  while (irecv > 0 && request[recvtotal] != 0)
    {
      if (strstr(request, "/r/n/r/n"))
        {
          break;
        }
      irecv = recv(clisock, &request + recvtotal, REQUEST_SIZE, 0);
      recvtotal += irecv;
    }
  shutdown(clisock, SHUT_RD);

  time_t time_t_receive = time(NULL);

  /* get date for response use */
  char *date = strdate();

  /* log request */
  if (server_config.recording == TRUE)
    {
      FILE *request_log_file = fopen(server_config.recordfile, "w");
      int i = 0;
      while (i < (int) strlen(request))
        {
          fputc(request[i], request_log_file);
          ++i;
        }
      fclose(request_log_file);
    }

  /* start building response */
  int status_code = 0;

  /* parse request, determing response code */
  char *req_filename = NULL;
  char *localfile = NULL;
  if (strstr(request, "GET ") == request)
    {
      /* get request filename */
      char *req_file_start = strchr(request, ' ') + 1;
      int filename_length = strcspn(req_file_start, " ");
      if (filename_length > 0)
        {
          req_filename = calloc(filename_length + 1, sizeof(char));
          strncpy(req_filename, req_file_start, filename_length);

          /* get local filename */
          localfile = calloc(
              strlen(server_config.root) + strlen(req_filename) + 1,
              sizeof(char));
          sprintf(localfile, "%s%s", server_config.root, req_filename);

          /* replace hexcode escaped characters with ascii counterparts */
          char *c = localfile;
          while (*c != '\0')
            {
              if (*c == '%')
                {
                  char num[3];
                  num[0] = *(c + 1);
                  num[1] = *(c + 2);
                  num[2] = '\0';

                  int hexchar = (int) strtol(num, NULL, 16);

                  char *therestofthestring = c + 3;

                  *c = hexchar;
                  *(c + 1) = '\0';

                  strcat(c, therestofthestring);
                }
              ++c;
            }

          /* on directories, append root document filename */
          if (localfile[strlen(localfile) - 1] == '/')
            {
              strcat(localfile, "index.html");
            }
        }
      /* test file exists */
      if (file_exists(localfile))
        {
          if (file_length(localfile) > 0)
            {
              status_code = HTTP_200_OK;
            }
          else
            {
              status_code = HTTP_204_No_Content;
            }
        }
      else
        {
          status_code = HTTP_404_Not_Found;
        }
    }
  else
    {
      status_code = HTTP_400_Bad_Request;
    }

  /* handle special requests */
  int special_request_type = 0;
  if (strcmp(req_filename + 1, server_config.shutdown_request) == 0)
    {
      kill(server_state.parent_pid, server_config.shutdown_signal);
      status_code = HTTP_200_OK;
      special_request_type = 1;
    }
  else if (strcmp(req_filename + 1, server_config.status_request) == 0)
    {
      status_code = HTTP_200_OK;
      special_request_type = 2;
    }

  /* build response */
  char response[RESPONSE_SIZE];
  unsigned char *content = NULL;
  int content_length = 0;
  switch (status_code)
    {
  case 200:
    if (special_request_type == 1)
      {
        sprintf(response, "HTTP/1.1 200 OK\r\n"
        "Date: %s\r\n"
        "Server: %s\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s", date, server_config.name, (int)strlen(docshutdown), docshutdown);
      }
    else if (special_request_type == 2)
      {
        content_length = strlen(docstatus_format) + 1024;
        content = calloc(content_length + 1, sizeof(char));

        sprintf(
            (char *)content,
            docstatus_format, server_config.name, date, server_state.connections, server_state.total_requests, server_config.port, server_config.shutdown_signal, server_state.parent_pid, server_config.shutdown_request);

        content_length = (int) strlen((char *) content);

        sprintf(response, "HTTP/1.1 200 OK\r\n"
        "Date: %s\r\n"
        "Server: %s\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n", date, server_config.name, content_length);

      }
    else
      {
        content_length = file_length(localfile);
        sprintf(
            response,
            "HTTP/1.1 200 OK\r\n"
            "Date: %s\r\n"
            "Server: %s\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n", date, server_config.name, get_content_type(localfile, server_config.mime_types), content_length);
        content = filegetc(localfile);
      }
    break;
  case 204:
    sprintf(
        response,
        "HTTP/1.1 204 No Data\r\n"
        "Date: %s\r\n"
        "Server: %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n", date, server_config.name, get_content_type(localfile, server_config.mime_types));
    break;
  case 400:
    sprintf(response, "HTTP/1.1 400 Bad Request\r\n"
    "Date: %s\r\n"
    "Server: %s\r\n"
    "Connection: close\r\n"
    "\r\n", date, server_config.name);
    break;
  case 404:
    content_length = strlen(doc404_format) + strlen(req_filename);
    content = calloc(content_length + 1, sizeof(char));

    sprintf(response, "HTTP/1.1 404 Not Found\r\n"
    "Date: %s\r\n"
    "Server: %s\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: %d\r\n"
    "Connection: close\r\n"
    "\r\n", date, server_config.name, content_length);

    sprintf((char *)content, doc404_format, req_filename);
    break;
  default:
    sprintf(response, "HTTP/1.1 501 Not Implemented\r\n"
    "Date: %s\r\n"
    "Server: %s\r\n"
    "Connection: close\r\n"
    "\r\n", date, server_config.name);
    break;
    }
  free(date);

  send(clisock, response, sizeof(char) * strlen(response), 0);
  if (content_length > 0)
    {
      send(clisock, content, content_length, 0);
      free(content);
    }

  /* log completion time */
  time_t time_t_finish = time(NULL);
  struct tm * tm_finish = localtime(&time_t_finish);
  log_write(time_t_receive, "%02d:%02d:%02d ip port %s %d %d 0", tm_finish->tm_hour, tm_finish->tm_min,
      tm_finish->tm_sec, req_filename, status_code, content_length);
  if (special_request_type == 1) {
      log_write(time(NULL), "shutdown request");
  }

  shutdown(clisock, SHUT_RDWR);
  close(clisock);
}
