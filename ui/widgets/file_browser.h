/**
 * @file    file_browser.h
 * @brief   文件浏览器网格控件 — 自定义可复用控件
 *
 * 以网格形式展示目录下的文件和子目录。
 * 支持图标+文件名显示，点击进入子目录或打开文件。
 *
 * Phase 6 将实现完整的文件浏览功能。
 */

#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H
#include "lvgl/lvgl.h"

lv_obj_t *file_browser_create(lv_obj_t *parent);
void file_browser_set_dir(lv_obj_t *browser, const char *path);

#endif
