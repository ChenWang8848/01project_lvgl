/**
 * @file    text_screen.c
 * @brief   文本阅读器 — 显示文本文件内容
 *
 * 布局 (800×480):
 *   ┌──────────────────────────────────┐
 *   │ ← 返回    文件名.txt              │  56px 标题栏
 *   ├──────────────────────────────────┤
 *   │  文件内容 (lv_textarea, 可滚动)    │  424px
 *   └──────────────────────────────────┘
 */

#include "text_screen.h"
#include "app/page_manager.h"
#include "service/file_service.h"
#include "ui/styles.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

text_screen_t *g_text_screen = NULL;

/* ================================================================
 *  事件
 * ================================================================ */
static void on_back_click(lv_event_t *e)
{
    (void)e;
    /* 返回文件浏览器 */
    page_manager_navigate("manual", NULL);
}

/* ================================================================
 *  构造
 * ================================================================ */
static lv_obj_t *text_screen_create(base_screen_t *base);
static void text_screen_destroy(base_screen_t *base);

void text_screen_init(text_screen_t *self)
{
    memset(self, 0, sizeof(*self));
    base_screen_init(&self->base, "text");
    self->base.create  = text_screen_create;
    self->base.destroy = text_screen_destroy;
    g_text_screen = self;
}

void text_screen_set_path(text_screen_t *self, const char *path)
{
    snprintf(self->file_path, sizeof(self->file_path), "%s", path);
}

/**
 * @brief 从文件名中提取文件名部分 (去掉路径前缀)
 */
static const char *base_name(const char *path)
{
    const char *p = strrchr(path, '/');
    return p ? p + 1 : path;
}

static lv_obj_t *text_screen_create(base_screen_t *base)
{
    text_screen_t *self = (text_screen_t *)base;

    /* 创建 screen */
    self->base.screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(self->base.screen,
                              lv_color_hex(0xE7DBB5), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->base.screen, 0, LV_PART_MAIN);
    lv_obj_clear_flag(self->base.screen, LV_OBJ_FLAG_SCROLLABLE);

    /* ---- 标题栏 ---- */
    self->title_bar = lv_obj_create(self->base.screen);
    lv_obj_set_style_bg_color(self->title_bar, lv_color_hex(0xD4C4A0), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->title_bar, 0, LV_PART_MAIN);
    lv_obj_clear_flag(self->title_bar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(self->title_bar, 0, 0);
    lv_obj_set_size(self->title_bar, 800, 56);

    /* 返回按钮 */
    self->back_btn = lv_btn_create(self->title_bar);
    lv_obj_set_style_bg_color(self->back_btn, lv_color_hex(0xC4B490), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->back_btn, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(self->back_btn, 10, LV_PART_MAIN);
    lv_obj_set_size(self->back_btn, 90, 40);
    lv_obj_set_pos(self->back_btn, 10, 8);
    lv_obj_clear_flag(self->back_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(self->back_btn, on_back_click, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_label = lv_label_create(self->back_btn);
    lv_label_set_text(btn_label, "← 返回");
    lv_obj_set_style_text_color(btn_label, lv_color_hex(0x5A4A3A), LV_PART_MAIN);
    if (font_cjk_16) lv_obj_set_style_text_font(btn_label, font_cjk_16, LV_PART_MAIN);
    lv_obj_center(btn_label);

    /* 标题: 文件名 */
    self->title_label = lv_label_create(self->title_bar);
    lv_label_set_text(self->title_label, "文本阅读");
    lv_obj_set_style_text_color(self->title_label, lv_color_hex(0x5A4A3A), LV_PART_MAIN);
    if (font_cjk_24) lv_obj_set_style_text_font(self->title_label, font_cjk_24, LV_PART_MAIN);
    lv_obj_set_pos(self->title_label, 110, 12);

    /* ---- 文本内容区域 ---- */
    self->text_area = lv_textarea_create(self->base.screen);
    lv_obj_set_pos(self->text_area, 10, 62);
    lv_obj_set_size(self->text_area, 780, 408);
    lv_obj_set_style_bg_color(self->text_area, lv_color_hex(0xF5F0E0), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->text_area, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(self->text_area, lv_color_hex(0xC4B490), LV_PART_MAIN);
    lv_obj_set_style_text_color(self->text_area, lv_color_hex(0x3D3222), LV_PART_MAIN);
    lv_obj_set_style_radius(self->text_area, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_all(self->text_area, 12, LV_PART_MAIN);
    lv_textarea_set_cursor_hidden(self->text_area, true);
    if (font_cjk_16) lv_obj_set_style_text_font(self->text_area, font_cjk_16, LV_PART_MAIN);

    /* ---- 加载文件内容 ---- */
    if (self->file_path[0]) {
        size_t len = 0;
        char *buf = file_read_text(self->file_path, &len);
        if (buf) {
            lv_textarea_set_text(self->text_area, buf);
            lv_label_set_text(self->title_label, base_name(self->file_path));
            free(buf);
        } else {
            lv_textarea_set_text(self->text_area, "(文件无法读取)");
            lv_label_set_text(self->title_label, "错误");
        }
    }

    lv_scr_load(self->base.screen);
    return self->base.screen;
}

static void text_screen_destroy(base_screen_t *base)
{
    text_screen_t *self = (text_screen_t *)base;
    if (self->base.screen) {
        lv_obj_del(self->base.screen);
        self->base.screen = NULL;
    }
}
