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

void
http_respond(int clisock);

#endif /* HTTP_H_ */
