/**
 * @file    auto_screen.h
 * @brief   自动播放页面 — 幻灯片播放入口 (Phase 6 实现具体功能)
 */
#ifndef AUTO_SCREEN_H
#define AUTO_SCREEN_H
#include "base_screen.h"

typedef struct {
    base_screen_t base;
    lv_obj_t     *title_bar;
    lv_obj_t     *back_btn;
    lv_obj_t     *title_label;
} auto_screen_t;

void auto_screen_init(auto_screen_t *self);
#endif
