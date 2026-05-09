/**
 * @file    setting_screen.h
 * @brief   设置页面 — 设置菜单入口 (Phase 6 实现具体功能)
 */
#ifndef SETTING_SCREEN_H
#define SETTING_SCREEN_H
#include "base_screen.h"

typedef struct {
    base_screen_t base;
    lv_obj_t     *title_bar;
    lv_obj_t     *back_btn;
    lv_obj_t     *title_label;
} setting_screen_t;

void setting_screen_init(setting_screen_t *self);
#endif
