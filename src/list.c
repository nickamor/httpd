//
// Created by Nicholas Amor on 6/4/17.
//

#include <stdlib.h>
#include "list.h"

struct list_t *list_new() {
    struct list_t* list = malloc(sizeof(struct list_t));

    list->data = NULL;
    list->next = NULL;

    return list;
}

void list_append(struct list_t **list, void *data) {
    struct list_t *new = list_new();
    new->data = data;

    if (*list != NULL) {
        list_tail(*list)->next = new;
    } else {
        *list = new;
    }
}

/* return the last item in a linked list */
struct list_t *list_tail(struct list_t *list) {
    if (list->next == NULL) {
        return list;
    }
    return list_tail(list->next);
}

void list_delete(struct list_t *list) {
    struct list_t *iter = list;

    while (iter != NULL) {
        struct list_t *next = iter->next;
        free(iter);
        iter = next;
    }
}
