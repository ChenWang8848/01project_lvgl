/**
 * @file    manual_screen.c
 * @brief   手动播放页面 — 返回按钮 + 空白内容区
 *
 * 布局:
 *   ┌──────────────────────────────┐
 *   │ ← 返回      手动播放          │  标题栏 56px
 *   ├──────────────────────────────┤
 *   │         空白内容区域          │
 *   └──────────────────────────────┘
 */

#include "manual_screen.h"
#include "app/page_manager.h"
#include "ui/styles.h"
#include <string.h>

/* ================================================================
 *  事件处理
 * ================================================================ */
static void on_back_click(lv_event_t *e)
{
    (void)e;
    page_manager_go_back();
}

/* ================================================================
 *  构造/析构
 * ================================================================ */
static lv_obj_t *manual_screen_create(base_screen_t *base);
static void manual_screen_destroy(base_screen_t *base);

void manual_screen_init(manual_screen_t *self)
{
    memset(self, 0, sizeof(*self));
    base_screen_init(&self->base, "manual");
    self->base.create  = manual_screen_create;
    self->base.destroy = manual_screen_destroy;
}

static lv_obj_t *manual_screen_create(base_screen_t *base)
{
    manual_screen_t *self = (manual_screen_t *)base;

    /* 创建 screen */
    self->base.screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(self->base.screen,
                              lv_color_hex(0x1A1A2E), LV_PART_MAIN);

    /* ---- 标题栏 ---- */
    self->title_bar = lv_obj_create(self->base.screen);
    lv_obj_add_style(self->title_bar, style_title_bar, LV_PART_MAIN);
    lv_obj_set_pos(self->title_bar, 0, 0);

    /* 返回按钮 ("← 返回") */
    self->back_btn = lv_btn_create(self->title_bar);
    lv_obj_add_style(self->back_btn, style_back_btn, LV_PART_MAIN);
    lv_obj_set_pos(self->back_btn, 10, 8);
    lv_obj_add_event_cb(self->back_btn, on_back_click, LV_EVENT_CLICKED, NULL);
    lv_obj_t *btn_label = lv_label_create(self->back_btn);
    lv_label_set_text(btn_label, "← 返回");
    lv_obj_set_style_text_color(btn_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_center(btn_label);

    /* 标题文字 */
    self->title_label = lv_label_create(self->title_bar);
    lv_label_set_text(self->title_label, "手动播放");
    lv_obj_add_style(self->title_label, style_title_text, LV_PART_MAIN);
    lv_obj_set_pos(self->title_label, 110, 12);

    lv_scr_load(self->base.screen);
    return self->base.screen;
}

static void manual_screen_destroy(base_screen_t *base)
{
    manual_screen_t *self = (manual_screen_t *)base;
    if (self->base.screen) {
        lv_obj_del(self->base.screen);
        self->base.screen = NULL;
    }
}
