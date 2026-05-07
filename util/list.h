#ifndef LIST_H
#define LIST_H

#include <stddef.h>

typedef struct list_node {
    struct list_node *prev;
    struct list_node *next;
} list_node_t;

typedef struct {
    list_node_t head;
    int count;
} list_t;

#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

void list_init(list_t *list);
void list_insert_tail(list_t *list, list_node_t *node);
void list_remove(list_node_t *node);
int  list_empty(list_t *list);
int  list_count(list_t *list);

#endif
