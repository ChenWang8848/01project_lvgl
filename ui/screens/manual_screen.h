/**
 * @file    manual_screen.h
 * @brief   手动播放页面 — 图片查看器入口 (Phase 6 实现具体功能)
 */
#ifndef MANUAL_SCREEN_H
#define MANUAL_SCREEN_H
#include "base_screen.h"

typedef struct {
    base_screen_t base;
    lv_obj_t     *title_bar;
    lv_obj_t     *back_btn;
    lv_obj_t     *title_label;
} manual_screen_t;

void manual_screen_init(manual_screen_t *self);
#endif
