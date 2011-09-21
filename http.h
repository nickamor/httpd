/*
 * http.h
 *
 *  Created on: Sep 21, 2011
 *      Author: nick
 */

#ifndef HTTP_H_
#define HTTP_H_

enum HTTP_RESPONSE_CODE
{
  HTTP_200_OK = 200,
  HTTP_204_No_Content = 204,
  HTTP_404_Not_Found = 404,
  HTTP_400_Bad_Request = 400
};

const char *doc404_format = "<html><head><title>File Not Found</title></head>\n"
    "<body><h3>File Not Found</h3>\n"
    "<p>The requested document %s was not found on this server.</p>\n"
    "</body></html>";


#endif /* HTTP_H_ */
