/**
 * @file    manual_screen.h
 * @brief   手动播放页面 — 文件浏览器 (图标网格 + 左侧工具栏)
 *
 * 布局 (800×480):
 *   ┌────┬──────────────────────────────────┐
 *   │工具栏│         文件图标网格 (4列×3行)    │
 *   │ 80px│        720px                     │
 *   ├退出┤  □  □  □  □                      │
 *   ├返回┤  dir file img music              │
 *   ├上一┤  □  □  □  □                      │
 *   ├下一页┤                                  │
 *   └────┴──────────────────────────────────┘
 */

#ifndef MANUAL_SCREEN_H
#define MANUAL_SCREEN_H
#include "base_screen.h"
#include "service/file_service.h"

#define MAX_DIR_ENTRIES  256    /**< 单目录最大条目数 */
#define ITEMS_PER_PAGE   12     /**< 每页显示条目数 (3行×4列) */

typedef struct {
    base_screen_t base;

    /* 标题栏 */
    lv_obj_t *title_bar;
    lv_obj_t *path_label;

    /* 左侧工具栏 (4个竖排按钮) */
    lv_obj_t *sidebar;
    lv_obj_t *btn_exit;        /**< 退出 → 主菜单 */
    lv_obj_t *btn_back;        /**< 返回 → 上级目录 */
    lv_obj_t *btn_prev;        /**< 上一页 */
    lv_obj_t *btn_next;        /**< 下一页 */

    /* 网格容器 */
    lv_obj_t *grid_cont;       /**< 图标网格容器 */

    /* 页面指示 */
    lv_obj_t *page_label;      /**< "第 1/5 页" */

    /* 状态 */
    char        cur_path[512];  /**< 当前目录路径 */
    int         cur_page;       /**< 当前页码 (从0开始) */
    int         total_entries;  /**< 当前目录条目总数 */
    dir_entry_t entries[MAX_DIR_ENTRIES]; /**< 当前目录全部条目 */

    /* 当前页面的图标对象 (最多12个) */
    lv_obj_t   *item_objs[ITEMS_PER_PAGE];
    lv_obj_t   *item_labels[ITEMS_PER_PAGE];
} manual_screen_t;

void manual_screen_init(manual_screen_t *self);

#endif
