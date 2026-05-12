/**
 * @file    auto_screen.c
 * @brief   图片查看器 — 布局与 text_screen 统一 (米黄主题)
 */

#include "auto_screen.h"
#include "app/page_manager.h"
#include "ui/styles.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

auto_screen_t *g_auto_screen = NULL;

static void on_back_click(lv_event_t *e);
static void on_prev_click(lv_event_t *e);
static void on_next_click(lv_event_t *e);

static const char *base_name(const char *path)
{ const char *p = strrchr(path, '/'); return p ? p + 1 : path; }

static void show_current(auto_screen_t *self)
{
    if (self->total_images == 0 || self->cur_index < 0) {
        lv_label_set_text(self->page_label, "0 / 0");
        return;
    }
    char fs_path[520];
    snprintf(fs_path, sizeof(fs_path), "S:%s", self->image_list[self->cur_index]);
    lv_img_set_src(self->img_obj, fs_path);
    lv_label_set_text(self->title_label, base_name(self->image_list[self->cur_index]));

    char buf[32];
    snprintf(buf, sizeof(buf), "%d / %d", self->cur_index + 1, self->total_images);
    lv_label_set_text(self->page_label, buf);
    lv_obj_set_style_bg_opa(self->btn_prev,
        (self->cur_index > 0) ? LV_OPA_COVER : LV_OPA_50, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(self->btn_next,
        (self->cur_index < self->total_images - 1) ? LV_OPA_COVER : LV_OPA_50, LV_PART_MAIN);
}

static void on_back_click(lv_event_t *e)
{ (void)e; page_manager_navigate("manual", NULL); }

static void on_prev_click(lv_event_t *e)
{ auto_screen_t *s = lv_event_get_user_data(e); if (s->cur_index > 0) { s->cur_index--; show_current(s); } }

static void on_next_click(lv_event_t *e)
{ auto_screen_t *s = lv_event_get_user_data(e); if (s->cur_index < s->total_images - 1) { s->cur_index++; show_current(s); } }

void auto_screen_open_images(auto_screen_t *self, const char (*list)[512], int count, int focus_idx)
{
    self->total_images = (count > MAX_IMAGES) ? MAX_IMAGES : count;
    for (int i = 0; i < self->total_images; i++)
        snprintf(self->image_list[i], sizeof(self->image_list[0]), "%s", list[i]);
    self->cur_index = (focus_idx >= 0 && focus_idx < self->total_images) ? focus_idx : 0;
}

static lv_obj_t *auto_screen_create(base_screen_t *base);
static void auto_screen_destroy(base_screen_t *base);

void auto_screen_init(auto_screen_t *self)
{
    memset(self, 0, sizeof(*self));
    base_screen_init(&self->base, "auto");
    self->base.create  = auto_screen_create;
    self->base.destroy = auto_screen_destroy;
    g_auto_screen = self;
}

static lv_obj_t *auto_screen_create(base_screen_t *base)
{
    auto_screen_t *self = (auto_screen_t *)base;

    /* screen (米黄, 与 text_screen 一致) */
    self->base.screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(self->base.screen, lv_color_hex(0xE7DBB5), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->base.screen, 0, LV_PART_MAIN);
    lv_obj_clear_flag(self->base.screen, LV_OBJ_FLAG_SCROLLABLE);

    /* 标题栏 (56px, 深米黄) */
    self->title_bar = lv_obj_create(self->base.screen);
    lv_obj_set_style_bg_color(self->title_bar, lv_color_hex(0xD4C4A0), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->title_bar, 0, LV_PART_MAIN);
    lv_obj_clear_flag(self->title_bar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(self->title_bar, 0, 0);
    lv_obj_set_size(self->title_bar, 800, 56);

    self->back_btn = lv_btn_create(self->title_bar);
    lv_obj_set_style_bg_color(self->back_btn, lv_color_hex(0xC4B490), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->back_btn, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(self->back_btn, 10, LV_PART_MAIN);
    lv_obj_set_size(self->back_btn, 90, 40);
    lv_obj_set_pos(self->back_btn, 10, 8);
    lv_obj_clear_flag(self->back_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(self->back_btn, on_back_click, LV_EVENT_CLICKED, NULL);
    lv_obj_t *bl = lv_label_create(self->back_btn);
    lv_label_set_text(bl, "← 返回");
    lv_obj_set_style_text_color(bl, lv_color_hex(0x5A4A3A), LV_PART_MAIN);
    if (font_cjk_16) lv_obj_set_style_text_font(bl, font_cjk_16, LV_PART_MAIN);
    lv_obj_center(bl);

    self->title_label = lv_label_create(self->title_bar);
    lv_label_set_text(self->title_label, "");
    lv_obj_set_style_text_color(self->title_label, lv_color_hex(0x5A4A3A), LV_PART_MAIN);
    if (font_cjk_24) lv_obj_set_style_text_font(self->title_label, font_cjk_24, LV_PART_MAIN);
    lv_obj_set_pos(self->title_label, 110, 12);

    /* 图片 (780×370, y=62, 与 text_label 一致) */
    self->img_obj = lv_img_create(self->base.screen);
    lv_obj_set_pos(self->img_obj, 10, 62);
    lv_obj_set_size(self->img_obj, 780, 370);
    lv_obj_set_style_bg_color(self->img_obj, lv_color_hex(0x3D3222), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(self->img_obj, LV_OPA_30, LV_PART_MAIN);
    lv_obj_set_style_radius(self->img_obj, 8, LV_PART_MAIN);

    /* 底栏 (44px, y=436) */
    self->bottom_bar = lv_obj_create(self->base.screen);
    lv_obj_set_style_bg_color(self->bottom_bar, lv_color_hex(0xD4C4A0), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->bottom_bar, 0, LV_PART_MAIN);
    lv_obj_clear_flag(self->bottom_bar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(self->bottom_bar, 0, 436);
    lv_obj_set_size(self->bottom_bar, 800, 44);

    /* 上一张 */
    self->btn_prev = lv_btn_create(self->bottom_bar);
    lv_obj_set_style_bg_color(self->btn_prev, lv_color_hex(0xC4B490), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->btn_prev, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(self->btn_prev, 8, LV_PART_MAIN);
    lv_obj_set_size(self->btn_prev, 100, 34);
    lv_obj_set_pos(self->btn_prev, 160, 5);
    lv_obj_add_event_cb(self->btn_prev, on_prev_click, LV_EVENT_CLICKED, self);
    lv_obj_t *pl = lv_label_create(self->btn_prev);
    lv_label_set_text(pl, "上一张");
    lv_obj_set_style_text_color(pl, lv_color_hex(0x5A4A3A), LV_PART_MAIN);
    if (font_cjk_16) lv_obj_set_style_text_font(pl, font_cjk_16, LV_PART_MAIN);
    lv_obj_center(pl);

    /* 下一张 */
    self->btn_next = lv_btn_create(self->bottom_bar);
    lv_obj_set_style_bg_color(self->btn_next, lv_color_hex(0xC4B490), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->btn_next, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(self->btn_next, 8, LV_PART_MAIN);
    lv_obj_set_size(self->btn_next, 100, 34);
    lv_obj_set_pos(self->btn_next, 540, 5);
    lv_obj_add_event_cb(self->btn_next, on_next_click, LV_EVENT_CLICKED, self);
    lv_obj_t *nl = lv_label_create(self->btn_next);
    lv_label_set_text(nl, "下一张");
    lv_obj_set_style_text_color(nl, lv_color_hex(0x5A4A3A), LV_PART_MAIN);
    if (font_cjk_16) lv_obj_set_style_text_font(nl, font_cjk_16, LV_PART_MAIN);
    lv_obj_center(nl);

    /* 页码 */
    self->page_label = lv_label_create(self->bottom_bar);
    lv_obj_set_style_text_color(self->page_label, lv_color_hex(0x5A4A3A), LV_PART_MAIN);
    if (font_cjk_16) lv_obj_set_style_text_font(self->page_label, font_cjk_16, LV_PART_MAIN);
    lv_obj_set_pos(self->page_label, 370, 10);

    show_current(self);
    lv_scr_load(self->base.screen);
    return self->base.screen;
}

static void auto_screen_destroy(base_screen_t *base)
{ auto_screen_t *s = (auto_screen_t *)base; if (s->base.screen) { lv_obj_del(s->base.screen); s->base.screen = NULL; } }
