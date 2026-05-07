/**
 * @file    main_screen.c
 * @brief   主菜单页面实现 — 显示 "hello lvgl"
 *
 * 作为 Phase 2 的验证页面，证明 LVGL 显示系统正常工作。
 *
 * create() 流程:
 *   1. lv_obj_create(NULL) 创建空白 screen
 *   2. 设置背景色 (深灰色 0x202020)
 *   3. 创建居中文字标签 "hello lvgl" (白色, 14pt 字体)
 *   4. lv_scr_load() 切换到当前 screen
 */

#include "main_screen.h"

static lv_obj_t *main_screen_create(base_screen_t *base);
static void main_screen_destroy(base_screen_t *base);

void main_screen_init(main_screen_t *self)
{
    memset(self, 0, sizeof(*self));
    base_screen_init(&self->base, "main");
    self->base.create  = main_screen_create;
    self->base.destroy = main_screen_destroy;
}

/**
 * @brief 构建主菜单页面的控件树
 *
 * 控件树结构:
 *   screen (深灰背景)
 *     └── label "hello lvgl" (白色文字, 居中)
 */
static lv_obj_t *main_screen_create(base_screen_t *base)
{
    main_screen_t *self = (main_screen_t *)base;

    /* 创建空白 screen */
    self->base.screen = lv_obj_create(NULL);

    /* 设置深灰背景 */
    lv_obj_set_style_bg_color(self->base.screen,
                              lv_color_hex(0x202020), LV_PART_MAIN);

    /* 创建居中标签 "hello lvgl" */
    self->label = lv_label_create(self->base.screen);
    lv_label_set_text(self->label, "hello lvgl");
    lv_obj_set_style_text_color(self->label,
                                lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(self->label,
                               &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_center(self->label);

    /* 加载为当前活动屏幕 */
    lv_scr_load(self->base.screen);

    return self->base.screen;
}

/**
 * @brief 销毁主菜单页面控件
 */
static void main_screen_destroy(base_screen_t *base)
{
    main_screen_t *self = (main_screen_t *)base;
    if (self->base.screen) {
        lv_obj_del(self->base.screen);
        self->base.screen = NULL;
    }
}
