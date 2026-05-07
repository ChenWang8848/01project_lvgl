/**
 * @file    list.h
 * @brief   通用双向链表 — Linux 内核风格 (侵入式链表)
 *
 * 设计要点:
 *   1. 链表节点嵌入在宿主结构体中 (侵入式)
 *   2. 使用 list_entry 宏从节点指针反推宿主指针 (类似 container_of)
 *   3. 头节点为哨兵节点 (不存储数据)
 *
 * 用法示例:
 *   struct my_data { int val; list_node_t node; };
 *   list_t list;
 *   list_init(&list);
 *   list_insert_tail(&list, &my_data.node);
 *   list_node_t *n; ... struct my_data *d = list_entry(n, struct my_data, node);
 */

#ifndef LIST_H
#define LIST_H

#include <stddef.h>

/** @brief 双向链表节点 */
typedef struct list_node {
    struct list_node *prev;     /**< 前驱节点 */
    struct list_node *next;     /**< 后继节点 */
} list_node_t;

/** @brief 双向链表 (哨兵头节点 + 计数) */
typedef struct {
    list_node_t head;           /**< 哨兵头节点 (不存储数据) */
    int count;                  /**< 链表节点数量 */
} list_t;

/**
 * @brief 从链表节点指针反推宿主结构体指针
 * @param ptr    链表节点指针
 * @param type   宿主结构体类型
 * @param member 链表节点在宿主结构体中的成员名
 *
 * 原理: 节点地址 - 节点在结构体中的偏移 = 结构体起始地址
 */
#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

void list_init(list_t *list);
void list_insert_tail(list_t *list, list_node_t *node);
void list_remove(list_node_t *node);
int  list_empty(list_t *list);
int  list_count(list_t *list);

#endif
