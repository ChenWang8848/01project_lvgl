/**
 * @file    list.c
 * @brief   通用双向链表实现
 *
 * 环形双向链表: head <-> node1 <-> node2 <-> ... <-> head
 * 哨兵 head 不存储数据，用于标记链表首尾。
 */

#include "list.h"

/**
 * @brief 初始化链表 — 哨兵节点的 prev/next 指向自身
 */
void list_init(list_t *list)
{
    list->head.prev = &list->head;
    list->head.next = &list->head;
    list->count = 0;
}

/**
 * @brief 在链表尾部插入节点
 *
 * 操作: head.prev <-> new_node <-> head
 */
void list_insert_tail(list_t *list, list_node_t *node)
{
    node->prev = list->head.prev;
    node->next = &list->head;
    list->head.prev->next = node;
    list->head.prev = node;
    list->count++;
}

/**
 * @brief 从链表中移除节点 (不释放内存)
 */
void list_remove(list_node_t *node)
{
    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    node->prev = NULL;
    node->next = NULL;
}

/**
 * @brief 判断链表是否为空 (仅哨兵节点)
 */
int list_empty(list_t *list)
{
    return list->head.next == &list->head;
}

/**
 * @brief 获取链表节点数量
 */
int list_count(list_t *list)
{
    return list->count;
}
