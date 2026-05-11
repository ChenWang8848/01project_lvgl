/**
 * @file    text_screen.h
 * @brief   文本阅读器 — 显示文本文件内容 (可滚动)
 */
#ifndef TEXT_SCREEN_H
#define TEXT_SCREEN_H
#include "base_screen.h"

typedef struct {
    base_screen_t base;
    lv_obj_t     *title_bar;
    lv_obj_t     *back_btn;
    lv_obj_t     *title_label;
    lv_obj_t     *text_area;      /**< LVGL 文本区域 (可滚动) */
    char         *content;        /**< 文件内容缓冲区 (malloc) */
    char          file_path[512]; /**< 当前显示的文件路径 */
} text_screen_t;

void text_screen_init(text_screen_t *self);
void text_screen_set_path(text_screen_t *self, const char *path);

/** 全局指针，供其他页面设置文件路径 (app.c 中初始化后赋值) */
extern text_screen_t *g_text_screen;

#endif
