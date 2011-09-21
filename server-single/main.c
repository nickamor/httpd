#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../common.h"
#include "../sockets.h"
#include "../http.h"

#define REQUEST_SIZE 1024
#define RESPONSE_SIZE 1024

int accepting = TRUE;

const char *httprootdir = "../httpdoc";

int
main(void);

void
stop_accepting(void);

void
stop_accepting()
{
  printf("\nExiting...\n");
  accepting = FALSE;
}

char *
get_content_type(char *filename)
{
  /* set content-type */
  if (strcmp(strrchr(filename, '.'), ".html") == 0)
    {
      return "text/html";
    }
  else if (strcmp(strrchr(filename, '.'), ".jpg") == 0)
    {
      return "image/jpeg";
    }
  else if (strcmp(strrchr(filename, '.'), ".png") == 0)
    {
      return "image/png";
    }
  else
    {
      return "text/plain";
    }
}

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

  /* get date for response use */
  char *date = strdate();

  /* log request */
  char *request_log = "request.txt";
  FILE *request_log_file = fopen(request_log, "w");
  int i = 0;
  while (i < (int) strlen(request))
    {
      fputc(request[i], request_log_file);
      ++i;
    }
  fclose(request_log_file);
  int response_code = 0;

  /* parse request */
  char *req_filename = NULL;
  char *localfile = NULL;
  if (strstr(request, "GET ") == request)
    {
      char *req_file_start = strchr(request, ' ') + 1;
      //char *req_file_end = strchr(req_file_start, ' ');

      int filename_length = strcspn(req_file_start, " ");
      if (filename_length > 0)
        {
          /* get request filename */
          req_filename = calloc(filename_length + 1, sizeof(char));
          strncpy(req_filename, req_file_start, filename_length);

          /* get local filename */
          localfile = calloc(strlen(httprootdir) + strlen(req_filename) + 1,
              sizeof(char));
          sprintf(localfile, "%s%s", httprootdir, req_filename);

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
    }
  else
    {
      response_code = HTTP_400_Bad_Request;
    }

  /* test file exists */
  if (file_exists(localfile))
    {
      if (file_length(localfile) > 0)
        {
          response_code = HTTP_200_OK;
        }
      else
        {
          response_code = HTTP_204_No_Content;
        }
    }
  else
    {
      response_code = HTTP_404_Not_Found;
    }
  char response[RESPONSE_SIZE];
  unsigned char *content = NULL;
  int content_length = 0;
  switch (response_code)
    {
  case 200:
    content_length = file_length(localfile);
    sprintf( response, "HTTP/1.1 200 OK\r\n"
    "Date: %s\r\n"
    "Connection: close\r\n"
    "Content-Type: %s\r\n"
    "Content-Length: %d\r\n"
    "\r\n", date, get_content_type(localfile), content_length);
    content = filegetc(localfile);
    break;
  case 204:
    sprintf(response, "HTTP/1.1 204 No Data\r\n"
    "Date: %s\r\n"
    "Connection: close\r\n"
    "Content-Type: %s\r\n"
    "Content-Length: 0\r\n"
    "\r\n", date, get_content_type(localfile));
    break;
  case 400:
    sprintf(response, "HTTP/1.1 400 Bad Request\r\n"
    "Date: %s\r\n"
    "Connection: close\r\n"
    "\r\n", date);
    break;
  case 404:
    content_length = strlen(doc404_format) + strlen(req_filename);
    content = calloc(content_length + 1, sizeof(char));

    sprintf(response, "HTTP/1.1 404 Not Found\r\n"
    "Date: %s\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: %d\r\n"
    "Connection: close\r\n"
    "\r\n", date, content_length);

    sprintf((char *)content, doc404_format, req_filename);
    break;
  default:
    sprintf(response, "HTTP/1.1 501 Not Implemented\r\n"
    "Date: %s\r\n"
    "Connection: close\r\n"
    "\r\n", date);
    break;
    }
  free(date);

  send(clisock, response, sizeof(char) * strlen(response), 0);
  if (content_length > 0)
    {
      send(clisock, content, content_length, 0);
      free(content);
    }

  shutdown(clisock, SHUT_RDWR);
  close(clisock);
}

int
main()
{
  int listen_socket = i_socket(PF_INET, SOCK_STREAM, 0);

  /* set the socket to be reusable */
  int on = 1;
  setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &on, sizeof(on));
  /* set the socket to timeout recv */
  /*
   struct timeval tm;
   tm.tv_sec = 30;
   tm.tv_usec = 0;
   setsockopt(listen_socket, SOL_SOCKET, SO_RCVTIMEO, (char *) &tm, sizeof(tm));
   */

  struct sockaddr_in myaddr;
  myaddr.sin_family = PF_INET;
  myaddr.sin_port = htons(40000);
  myaddr.sin_addr.s_addr = INADDR_ANY;

  i_bind(listen_socket, (struct sockaddr *) &myaddr, sizeof(myaddr));

  i_listen(listen_socket, 1024);

  /* register interrupt handler */
  struct sigaction new_action;
  new_action.sa_handler = (void *) stop_accepting;
  sigemptyset(&new_action.sa_mask);
  new_action.sa_flags = 0;
  int signal_to_listen_for = SIGTERM;
  sigaction(signal_to_listen_for, &new_action, NULL);

  while (accepting)
    {
      /* accept new connections */
      struct sockaddr_in cliaddr;
      memset(&cliaddr, 0, sizeof(cliaddr));
      socklen_t cliaddrlen = sizeof(cliaddr);
      int clisock = i_accept(listen_socket, (struct sockaddr *) &cliaddr,
          &cliaddrlen);
      if (clisock > 0)
        {
          /* receive bytes */
          http_respond(clisock);
        }
    }

  close(listen_socket);

  return EXIT_SUCCESS;
}
