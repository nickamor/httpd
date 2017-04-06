/*
 * childpid.h
 *
 *  Created on: Oct 11, 2011
 *      Author: nick
 */

#include <sys/types.h>

#ifndef CHILDPID_H_
#define CHILDPID_H_

struct childpid_list_t {
    pid_t pid;
    struct childpid_list_t *next;
};

struct childpid_list_t *childpid_list;


void add_childpid(pid_t childpid);

void remove_childpid(pid_t childpid);


#endif /* CHILDPID_H_ */
