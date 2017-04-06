//
// Created by Nicholas Amor on 6/4/17.
//

#ifndef HTTPD_LIST_H
#define HTTPD_LIST_H

struct list_t {
    void *data;
    struct list_t *next;
};

struct list_t *list_new();

void list_append(struct list_t **list, void *data);

struct list_t *list_tail(struct list_t *list);

void list_delete(struct list_t *list);

#endif //HTTPD_LIST_H
