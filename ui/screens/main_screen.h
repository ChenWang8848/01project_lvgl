/**
 * @file    main_screen.h
 * @brief   主菜单页面 — 手机桌面风格图标网格 (2行×6列)
 *
 * 继承自 base_screen_t。
 * 当前展示3个功能入口: 手动播放 / 自动播放 / 设置。
 * 每项 = 90×90 彩色圆角图标 + 下方文字标签。
 */

#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include "base_screen.h"

typedef struct {
    base_screen_t base;         /**< 父类 */
} main_screen_t;

void main_screen_init(main_screen_t *self);

#endif
