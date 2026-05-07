#include "list.h"

void list_init(list_t *list)
{
    list->head.prev = &list->head;
    list->head.next = &list->head;
    list->count = 0;
}

void list_insert_tail(list_t *list, list_node_t *node)
{
    node->prev = list->head.prev;
    node->next = &list->head;
    list->head.prev->next = node;
    list->head.prev = node;
    list->count++;
}

void list_remove(list_node_t *node)
{
    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    node->prev = NULL;
    node->next = NULL;
}

int list_empty(list_t *list)
{
    return list->head.next == &list->head;
}

int list_count(list_t *list)
{
    return list->count;
}
