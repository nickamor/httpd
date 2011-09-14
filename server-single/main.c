#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../utilities.h"

#define TRUE 1
#define FALSE 0

#define REQUEST_SIZE 1024
#define RESPONSE_SIZE 1024

int accepting = TRUE;

int
main(void);

void
sigint_handler(void);

void
sigint_handler()
{
  printf("\nExiting...\n");
  accepting = FALSE;
}

int
main()
{
  int listen_socket = i_socket(PF_INET, SOCK_STREAM, 0);

  /* set the socket to be reusable */
  int on = 1;
  setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &on, sizeof(on));
  /* set the socket to timeout recv */
  struct timeval tm;
  tm.tv_sec = 5;
  tm.tv_usec = 0;
  setsockopt(listen_socket, SOL_SOCKET, SO_RCVTIMEO, (char *) &tm, sizeof(tm));

  struct sockaddr_in myaddr;
  myaddr.sin_family = PF_INET;
  myaddr.sin_port = htons(40000);
  myaddr.sin_addr.s_addr = INADDR_ANY;

  i_bind(listen_socket, (struct sockaddr *) &myaddr, sizeof(myaddr));

  i_listen(listen_socket, 1024);

  /* register interrupt handler */
  struct sigaction new_action;
  new_action.sa_handler = (void *) sigint_handler;
  sigemptyset(&new_action.sa_mask);
  new_action.sa_flags = 0;
  sigaction(SIGINT, &new_action, NULL);
  sigaction(SIGTERM, &new_action, NULL);

  while (accepting)
    {
      /* accept new connections */
      struct sockaddr_in cliaddr;
      memset(&cliaddr, 0, sizeof(cliaddr));
      socklen_t cliaddrlen = sizeof(cliaddr);
      int clisock = i_accept(listen_socket, (struct sockaddr *) &cliaddr,
          &cliaddrlen);
      if (clisock == -1)
        {
          continue;
        }

      /* Receive bytes */
      int irecv = 0;
      char request[REQUEST_SIZE];
      memset(&request, 0, REQUEST_SIZE);
      irecv = recv(clisock, &request, REQUEST_SIZE, 0);
      int recvtotal = irecv;
      while (accepting && irecv > 0 && request[recvtotal] != 0)
        {
          if (strstr(request, "/r/n/r/n"))
            {
              break;
            }
          irecv = recv(clisock, &request + recvtotal, REQUEST_SIZE, 0);
          recvtotal += irecv;
        }
      shutdown(clisock, SHUT_RD);

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

      /* Validate request */
      int valid_request = TRUE;
      char getfile[80];
      memset(getfile, 0, 80);
      if (strstr(request, "GET ") == request)
        {
          char *req_file_start = strchr(request, ' ') + 1;
          char *req_file_end = strchr(req_file_start, ' ');

          int filename_length = strcspn(req_file_start, " ");
          strncpy(getfile, req_file_start, filename_length);

          if (strstr(req_file_end, "HTTP/1.1"))
            {
            }
          else if (strstr(req_file_end, "HTTP/1.0"))
            {
            }
          else
            {
              valid_request = FALSE;
            }
        }

      if (valid_request)
        {
          /* parse file request */
          char filename_to_retrieve[255];
          memset(filename_to_retrieve, 0, 255);
          char *httprootdir = "../httpdoc";
          strcat(filename_to_retrieve, httprootdir);
          strcat(filename_to_retrieve, getfile);
          if (filename_to_retrieve[strlen(filename_to_retrieve) - 1] == '/')
            {
              strcat(filename_to_retrieve, "index.html");
            }

          /* test file exists */
          int file_exists = TRUE;
          FILE *content_file = fopen(filename_to_retrieve, "r");
          if (!content_file)
            {
              file_exists = FALSE;
            }

          /* get date */
          time_t mytime = time(NULL);
          struct tm *mytimet = localtime(&mytime);
          char date[80];
          memset(date, 0, 80);
          strftime(date, 80, "%a, %d %b %Y %X %Z", mytimet);

          /* get content length */
          struct stat stbuf;
          stat(filename_to_retrieve, &stbuf);
          int content_length = (int) stbuf.st_size;

          /* get status */
          char *status;
          if (content_length > 0)
            {
              status = "200 OK";
            }
          else
            {
              status = "204 No Content";
            }

          /* set content-type */
          char *content_type;
          if (strcmp(strrchr(filename_to_retrieve, '.'), ".html") == 0)
            {
              content_type = "text/html";
            }
          else if (strcmp(strrchr(filename_to_retrieve, '.'), ".jpg") == 0)
            {
              content_type = "image/jpeg";
            }
          else if (strcmp(strrchr(filename_to_retrieve, '.'), ".png") == 0)
            {
              content_type = "image/png";
            }
          else
            {
              content_type = "text/plain";
            }

          /* build response header */
          char response[RESPONSE_SIZE];
          memset(response, 0, sizeof(char) * RESPONSE_SIZE);
          sprintf(response, "HTTP/1.1 %s\r\n"
          "Date: %s\r\n"
          "Connection: close\r\n"
          "Content-Type: %s\r\n"
          "Content-Length: %d\r\n"
          "\r\n", status, date, content_type, content_length);

          send(clisock, response, sizeof(char) * strlen(response), 0);

          /* read and send file */
          int bytes_read = 0;
          if (file_exists && content_length > 0)
            {
              char *content = calloc(content_length, sizeof(char));
              while (bytes_read < content_length)
                {
                  content[bytes_read] = fgetc(content_file);
                  ++bytes_read;
                }
              fclose(content_file);
              send(clisock, content, content_length, 0);
              free(content);
            }
        }
      else
        {
          char *response = "HTTP/1.1 400 Bad Request\r\n"
              "\r\n";
          send(clisock, response, sizeof(char) * strlen(response), 0);
        }

      shutdown(clisock, SHUT_RDWR);
      close(clisock);
    }

  close(listen_socket);

  return EXIT_SUCCESS;
}
