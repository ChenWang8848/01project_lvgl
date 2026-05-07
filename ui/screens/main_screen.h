/**
 * @file    main_screen.h
 * @brief   主菜单页面 — 程序启动后的第一个页面
 *
 * 继承自 base_screen_t。
 * 当前显示 "hello lvgl" 文字标签。
 * Phase 5+ 将扩展为包含功能入口按钮的完整主菜单。
 */

#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include "base_screen.h"

typedef struct {
    base_screen_t base;         /**< 父类 */
    lv_obj_t *label;            /**< hello lvgl 标签控件 */
} main_screen_t;

void main_screen_init(main_screen_t *self);

#endif
