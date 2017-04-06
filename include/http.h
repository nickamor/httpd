/*
 * http.h
 *
 *  Created on: Sep 21, 2011
 *      Author: nick
 */

#ifndef HTTP_H_
#define HTTP_H_

enum HTTP_RESPONSE_CODE {
    HTTP_200_OK = 200,
    HTTP_204_No_Content = 204,
    HTTP_305_Use_Proxy = 305,
    HTTP_404_Not_Found = 404,
    HTTP_400_Bad_Request = 400
};

static char *const header_shutdown =
        "HTTP/1.1 200 OK\r\n"
                "Date: %s\r\n"
                "Server: %s\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: %d\r\n"
                "Connection: close\r\n"
                "\r\n"
                "%s";

static char *const doc404_format =
        "<html><head><title>File Not Found</title></head>"
                "<body><h3>File Not Found</h3><p>The requested document %s was not found"
                " on this server.";

static char *const docshutdown =
        "<html><head><title>Server Shutting Down...</title></head>"
                "<body><h3>Server Shutting Down...";

static char *const docstatus_format =
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

void http_respond(int clisock);

#endif /* HTTP_H_ */
