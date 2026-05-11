/**
 * @file    text_screen.c
 * @brief   仿电子书分页文本阅读器
 *
 * 布局 (800×480):
 *   ┌──────────────────────────────────┐
 *   │ ← 返回    文件名.txt              │  56px 标题栏
 *   ├──────────────────────────────────┤
 *   │  第N页文本 (lv_label, 760x370)    │  内容区
 *   ├──────────────────────────────────┤
 *   │   上一页      3/15      下一页    │  44px 底栏
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
 *  UTF-8 辅助
 * ================================================================ */

/** @brief 计算 UTF-8 字符的字节长度 (首字节 → 1/2/3/4) */
static int utf8_char_len(unsigned char c)
{
    if (c < 0x80) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1; /* 非法字节, 按单字节处理 */
}

/**
 * @brief 返回从 buf 开始向前调整到合法 UTF-8 边界的偏移量
 *
 * 中文每页约 500~700 个 UTF-8 字符 (约1500字节),
 * 但为确保不超出屏幕, 使用 ~1200 字节/页
 */
#define BYTES_PER_PAGE 1200

/**
 * @brief 在 buf 中从 offset 向前调整到最近的换行或 UTF-8 边界
 * @return 调整后的偏移 (page_start)
 */
static int align_to_boundary(const char *buf, int offset, int total)
{
    if (offset >= total) return total;

    /* 优先对齐到换行 */
    int search = offset;
    while (search > 0 && buf[search] != '\n') search--;
    if (buf[search] == '\n' && search > 0) return search + 1; /* 跳过换行 */

    /* 没找到换行, 对齐到 UTF-8 边界 */
    search = offset;
    while (search > 0) {
        unsigned char c = (unsigned char)buf[search];
        if ((c & 0xC0) != 0x80) break; /* 找到起始字节 */
        search--;
    }
    return search;
}

/* ================================================================
 *  分页
 * ================================================================ */

static void build_pages(text_screen_t *self)
{
    self->page_count = 0;
    int total = strlen(self->full_text);
    if (total == 0) return;

    int pos = 0;
    while (pos < total && self->page_count < TEXT_MAX_PAGES) {
        self->page_starts[self->page_count] = pos;
        self->page_count++;
        int next = pos + BYTES_PER_PAGE;
        if (next >= total) break;
        pos = align_to_boundary(self->full_text, next, total);
    }
}

/* ================================================================
 *  显示当前页
 * ================================================================ */

static void show_page(text_screen_t *self)
{
    if (!self->full_text || self->page_count == 0) {
        lv_label_set_text(self->text_label, "(空文件)");
        lv_label_set_text(self->page_label, "0 / 0");
        return;
    }

    int start = self->page_starts[self->cur_page];
    int end   = (self->cur_page + 1 < self->page_count)
               ? self->page_starts[self->cur_page + 1]
               : strlen(self->full_text);

    /* 截取当前页文本 (临时 null-terminate) */
    char saved = self->full_text[end];
    self->full_text[end] = '\0';
    lv_label_set_text(self->text_label, self->full_text + start);
    self->full_text[end] = saved;

    /* 更新页码 */
    char buf[32];
    snprintf(buf, sizeof(buf), "%d / %d", self->cur_page + 1, self->page_count);
    lv_label_set_text(self->page_label, buf);

    /* 按钮可用状态 */
    lv_obj_set_style_bg_opa(self->btn_prev,
        (self->cur_page > 0) ? LV_OPA_COVER : LV_OPA_50, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(self->btn_next,
        (self->cur_page < self->page_count - 1) ? LV_OPA_COVER : LV_OPA_50, LV_PART_MAIN);
}

/* ================================================================
 *  事件
 * ================================================================ */

static void on_back_click(lv_event_t *e)
{
    (void)e;
    page_manager_navigate("manual", NULL);
}

static void on_prev_click(lv_event_t *e)
{
    text_screen_t *s = (text_screen_t *)lv_event_get_user_data(e);
    if (s->cur_page > 0) { s->cur_page--; show_page(s); }
}

static void on_next_click(lv_event_t *e)
{
    text_screen_t *s = (text_screen_t *)lv_event_get_user_data(e);
    if (s->cur_page < s->page_count - 1) { s->cur_page++; show_page(s); }
}

/* ================================================================
 *  构造/析构
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

static const char *base_name(const char *path)
{
    const char *p = strrchr(path, '/');
    return p ? p + 1 : path;
}

static lv_obj_t *text_screen_create(base_screen_t *base)
{
    text_screen_t *self = (text_screen_t *)base;

    /* screen */
    self->base.screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(self->base.screen, lv_color_hex(0xE7DBB5), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->base.screen, 0, LV_PART_MAIN);
    lv_obj_clear_flag(self->base.screen, LV_OBJ_FLAG_SCROLLABLE);

    /* ---- 标题栏 ---- */
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
    lv_label_set_text(self->title_label, "文本阅读");
    lv_obj_set_style_text_color(self->title_label, lv_color_hex(0x5A4A3A), LV_PART_MAIN);
    if (font_cjk_24) lv_obj_set_style_text_font(self->title_label, font_cjk_24, LV_PART_MAIN);
    lv_obj_set_pos(self->title_label, 110, 12);

    /* ---- 底部栏 ---- */
    self->bottom_bar = lv_obj_create(self->base.screen);
    lv_obj_set_style_bg_color(self->bottom_bar, lv_color_hex(0xD4C4A0), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->bottom_bar, 0, LV_PART_MAIN);
    lv_obj_clear_flag(self->bottom_bar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(self->bottom_bar, 0, 436);
    lv_obj_set_size(self->bottom_bar, 800, 44);

    /* 上一页按钮 */
    self->btn_prev = lv_btn_create(self->bottom_bar);
    lv_obj_set_style_bg_color(self->btn_prev, lv_color_hex(0xC4B490), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->btn_prev, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(self->btn_prev, 8, LV_PART_MAIN);
    lv_obj_set_size(self->btn_prev, 100, 34);
    lv_obj_set_pos(self->btn_prev, 160, 5);
    lv_obj_clear_flag(self->btn_prev, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(self->btn_prev, on_prev_click, LV_EVENT_CLICKED, self);

    lv_obj_t *pl = lv_label_create(self->btn_prev);
    lv_label_set_text(pl, "上一页");
    lv_obj_set_style_text_color(pl, lv_color_hex(0x5A4A3A), LV_PART_MAIN);
    if (font_cjk_16) lv_obj_set_style_text_font(pl, font_cjk_16, LV_PART_MAIN);
    lv_obj_center(pl);

    /* 下一页按钮 */
    self->btn_next = lv_btn_create(self->bottom_bar);
    lv_obj_set_style_bg_color(self->btn_next, lv_color_hex(0xC4B490), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->btn_next, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(self->btn_next, 8, LV_PART_MAIN);
    lv_obj_set_size(self->btn_next, 100, 34);
    lv_obj_set_pos(self->btn_next, 540, 5);
    lv_obj_clear_flag(self->btn_next, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(self->btn_next, on_next_click, LV_EVENT_CLICKED, self);

    lv_obj_t *nl = lv_label_create(self->btn_next);
    lv_label_set_text(nl, "下一页");
    lv_obj_set_style_text_color(nl, lv_color_hex(0x5A4A3A), LV_PART_MAIN);
    if (font_cjk_16) lv_obj_set_style_text_font(nl, font_cjk_16, LV_PART_MAIN);
    lv_obj_center(nl);

    /* 页码 */
    self->page_label = lv_label_create(self->bottom_bar);
    lv_label_set_text(self->page_label, "0 / 0");
    lv_obj_set_style_text_color(self->page_label, lv_color_hex(0x5A4A3A), LV_PART_MAIN);
    if (font_cjk_16) lv_obj_set_style_text_font(self->page_label, font_cjk_16, LV_PART_MAIN);
    lv_obj_set_pos(self->page_label, 370, 10);

    /* ---- 文本内容 (lv_label) ---- */
    self->text_label = lv_label_create(self->base.screen);
    lv_obj_set_pos(self->text_label, 10, 62);
    lv_obj_set_size(self->text_label, 780, 370);
    lv_obj_set_style_text_color(self->text_label, lv_color_hex(0x3D3222), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(self->text_label, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_label_set_long_mode(self->text_label, LV_LABEL_LONG_WRAP);
    if (font_cjk_16) lv_obj_set_style_text_font(self->text_label, font_cjk_16, LV_PART_MAIN);

    /* ---- 加载并分页 ---- */
    if (self->file_path[0]) {
        /* 释放旧内容 */
        if (self->full_text) { free(self->full_text); self->full_text = NULL; }

        size_t len = 0;
        self->full_text = file_read_text(self->file_path, &len);
        if (self->full_text) {
            build_pages(self);
            self->cur_page = 0;
            show_page(self);
            lv_label_set_text(self->title_label, base_name(self->file_path));
        } else {
            lv_label_set_text(self->text_label, "(文件无法读取)");
            self->page_count = 0;
        }
    }

    lv_scr_load(self->base.screen);
    return self->base.screen;
}

static void text_screen_destroy(base_screen_t *base)
{
    text_screen_t *self = (text_screen_t *)base;
    if (self->full_text) { free(self->full_text); self->full_text = NULL; }
    if (self->base.screen) {
        lv_obj_del(self->base.screen);
        self->base.screen = NULL;
    }
}
