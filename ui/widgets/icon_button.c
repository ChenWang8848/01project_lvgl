/**
 * @file    icon_button.c
 * @brief   图标文字按钮实现
 *
 * 当前为最简实现。Phase 5 将扩展:
 *   1. 添加图标 (lv_img) 在文字左侧
 *   2. 统一按钮尺寸和圆角
 *   3. 点击动画反馈
 */

#include "icon_button.h"

lv_obj_t *icon_button_create(lv_obj_t *parent, const char *text)
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    return btn;
}
