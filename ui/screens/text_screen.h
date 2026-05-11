/**
 * @file    text_screen.h
 * @brief   文本阅读器 — 仿电子书分页显示
 *
 * 每次显示一页，通过上一页/下一页按钮翻页，不使用滚动。
 */
#ifndef TEXT_SCREEN_H
#define TEXT_SCREEN_H
#include "base_screen.h"

#define TEXT_MAX_PAGES 512       /**< 最大页数 */

typedef struct {
    base_screen_t base;

    /* 标题栏 */
    lv_obj_t *title_bar;
    lv_obj_t *back_btn;
    lv_obj_t *title_label;

    /* 底部工具栏 */
    lv_obj_t *bottom_bar;
    lv_obj_t *btn_prev;         /**< 上一页 */
    lv_obj_t *btn_next;         /**< 下一页 */
    lv_obj_t *page_label;       /**< "3 / 15" */

    /* 内容 */
    lv_obj_t *text_label;       /**< 当前页文字 (lv_label) */
    char     *full_text;        /**< 全文缓冲区 */
    int       total_pages;      /**< 总页数 */
    int       cur_page;         /**< 当前页码 (从0起) */

    /* 每页在全文中的偏移 (用于切页) */
    int       page_starts[TEXT_MAX_PAGES];
    int       page_count;

    char      file_path[512];
} text_screen_t;

void text_screen_init(text_screen_t *self);
void text_screen_set_path(text_screen_t *self, const char *path);
extern text_screen_t *g_text_screen;

#endif
