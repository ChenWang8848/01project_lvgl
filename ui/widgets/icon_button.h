/**
 * @file    icon_button.h
 * @brief   图标+文字按钮 — 自定义可复用控件
 *
 * 封装 LVGL btn + label，提供统一风格的图标文字按钮。
 * 用于主菜单和设置页面的功能入口。
 */

#ifndef ICON_BUTTON_H
#define ICON_BUTTON_H
#include "lvgl/lvgl.h"

/**
 * @brief 创建图标文字按钮
 * @param parent 父容器
 * @param text   按钮文字
 * @return 按钮对象 (lv_obj_t*)
 */
lv_obj_t *icon_button_create(lv_obj_t *parent, const char *text);

#endif
