/**
 * @file    auto_screen.c
 * @brief   图片查看器 — 多线程预加载 + 缓存, 米黄主题
 *
 * 翻页时优先从预加载缓存读取, 瞬时显示。
 * 后台 2 个工作线程同时解码上一张和下一张。
 */

#include "auto_screen.h"
#include "app/page_manager.h"
#include "service/image_cache.h"
#include "ui/styles.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

auto_screen_t *g_auto_screen = NULL;

/* ================================================================
 *  布局常量 (修改这些宏即可调整整体界面)
 * ================================================================ */
#define SCREEN_W        800
#define SCREEN_H        480

#define TITLE_BAR_H     56
#define BOTTOM_BAR_H    44

#define GAP_TITLE_IMG   6    /* 标题栏底 → 图片顶 */
#define GAP_IMG_BOTTOM  4    /* 图片底 → 底栏顶 */

#define IMG_AREA_X      10
#define IMG_AREA_Y      (TITLE_BAR_H + GAP_TITLE_IMG)
#define IMG_AREA_W      (SCREEN_W - IMG_AREA_X * 2)
#define IMG_AREA_H      (SCREEN_H - TITLE_BAR_H - BOTTOM_BAR_H - GAP_TITLE_IMG - GAP_IMG_BOTTOM)
#define IMG_RADIUS      8

#define BOTTOM_BAR_Y    (SCREEN_H - BOTTOM_BAR_H)

/* 返回按钮: 标题栏内 */
#define BACK_BTN_W      90
#define BACK_BTN_H      40
#define BACK_BTN_X      10
#define BACK_BTN_Y      ((TITLE_BAR_H - BACK_BTN_H) / 2)
#define BACK_BTN_R      10

/* 标题文本 */
#define TITLE_LABEL_X   110
#define TITLE_LABEL_Y   12

/* 上一张 / 下一张: 底栏内 */
#define NAV_BTN_W       100
#define NAV_BTN_H       (BOTTOM_BAR_H - 10)
#define NAV_BTN_Y       5
#define NAV_BTN_R       8
#define BTN_PREV_X      160
#define BTN_NEXT_X      (SCREEN_W - BTN_PREV_X - NAV_BTN_W)

/* 页码 */
#define PAGE_LABEL_X    370
#define PAGE_LABEL_Y    10

/* ---- 事件 ---- */
static void on_back_click(lv_event_t *e);
static void on_prev_click(lv_event_t *e);
static void on_next_click(lv_event_t *e);

/* ---- 辅助 ---- */
static const char *base_name(const char *path)
{ const char *p = strrchr(path, '/'); return p ? p + 1 : path; } 

/* ================================================================
 *  预加载线程
 * ================================================================ */
static pthread_mutex_t g_decode_mutex = PTHREAD_MUTEX_INITIALIZER;

static void *preload_worker(void *arg)
{
    char *path = (char *)arg;

    /* 先检查缓存是否已有 */
    uint32_t w, h;
    uint8_t cf;
    if (image_cache_lookup(path, &w, &h, &cf)) {
        free(path);
        return NULL;
    }

    /* 使用 LVGL 解码器解码 (加锁保证线程安全) */
    char fs_path[520];
    snprintf(fs_path, sizeof(fs_path), "S:%s", path);

    pthread_mutex_lock(&g_decode_mutex);

    lv_img_header_t header;
    lv_res_t res = lv_img_decoder_get_info(fs_path, &header);
    if (res != LV_RES_OK || header.w == 0 || header.h == 0) {
        pthread_mutex_unlock(&g_decode_mutex);
        free(path);
        return NULL;
    }

    uint8_t px_size = lv_img_cf_get_px_size(header.cf) >> 3;
    if (px_size == 0) px_size = 4;
    uint32_t data_size = (uint32_t)header.w * header.h * px_size;
    if (data_size > 32 * 1024 * 1024) {  /* 大图不预加载, 回退同步解码 */
        pthread_mutex_unlock(&g_decode_mutex);
        free(path);
        return NULL;
    }

    uint8_t *pixels = malloc(data_size);
    if (!pixels) {
        pthread_mutex_unlock(&g_decode_mutex);
        free(path);
        return NULL;
    }

    lv_img_decoder_dsc_t dsc;
    res = lv_img_decoder_open(&dsc, fs_path, lv_color_black(), 0);
    if (res != LV_RES_OK) {
        pthread_mutex_unlock(&g_decode_mutex);
        free(pixels);
        free(path);
        return NULL;
    }

    /* open 后 dsc.header.w/h 为解码后真实尺寸, 但 cf 仍是 info 阶段设的 RAW 标记.
     * 像素已按系统色深转换, 直接按 4 字节/像素拷贝. */
    uint8_t real_px = lv_img_cf_get_px_size(dsc.header.cf) >> 3;
    if (real_px == 0) real_px = 4;
    uint32_t real_size = (uint32_t)dsc.header.w * dsc.header.h * real_px;

    if (real_size != data_size) {
        /* 解码后尺寸与预估不同, 重新分配 */
        free(pixels);
        pixels = malloc(real_size);
        if (!pixels) {
            lv_img_decoder_close(&dsc);
            pthread_mutex_unlock(&g_decode_mutex);
            free(path);
            return NULL;
        }
        data_size = real_size;
        px_size = real_px;
    }

    if (dsc.img_data) {
        memcpy(pixels, dsc.img_data, data_size);
    } else {
        uint8_t *dst = pixels;
        for (uint32_t y = 0; y < dsc.header.h; y++) {
            lv_res_t line_res = lv_img_decoder_read_line(&dsc, 0, (lv_coord_t)y,
                                                          dsc.header.w, dst);
            if (line_res != LV_RES_OK) break;
            dst += dsc.header.w * px_size;
        }
    }

    /* 计算目标尺寸: 等比缩放至图片容器内, 不放大 */
    uint32_t tw = dsc.header.w;
    uint32_t th = dsc.header.h;
    if (tw > IMG_AREA_W || th > IMG_AREA_H) {
        uint32_t zw = (IMG_AREA_W * 256) / tw;
        uint32_t zh = (IMG_AREA_H * 256) / th;
        uint32_t z = (zw < zh) ? zw : zh;
        tw = (tw * z) >> 8;
        th = (th * z) >> 8;
    }

    /* 降采样到目标尺寸 (box filter) */
    if (tw != (uint32_t)dsc.header.w || th != (uint32_t)dsc.header.h) {
        uint32_t scaled_size = tw * th * px_size;
        uint8_t *scaled = malloc(scaled_size);
        if (scaled) {
            for (uint32_t dy = 0; dy < th; dy++) {
                uint32_t sy0 = dy * dsc.header.h / th;
                uint32_t sy1 = (dy + 1) * dsc.header.h / th;
                for (uint32_t dx = 0; dx < tw; dx++) {
                    uint32_t sx0 = dx * dsc.header.w / tw;
                    uint32_t sx1 = (dx + 1) * dsc.header.w / tw;
                    uint32_t r = 0, g = 0, b = 0, a = 0, cnt = 0;
                    for (uint32_t sy = sy0; sy < sy1; sy++) {
                        const uint8_t *row = pixels + sy * dsc.header.w * px_size;
                        for (uint32_t sx = sx0; sx < sx1; sx++) {
                            const uint8_t *p = row + sx * px_size;
                            r += p[0]; g += p[1]; b += p[2]; a += p[3];
                            cnt++;
                        }
                    }
                    uint8_t *d = scaled + (dy * tw + dx) * px_size;
                    d[0] = (uint8_t)(r / cnt);
                    d[1] = (uint8_t)(g / cnt);
                    d[2] = (uint8_t)(b / cnt);
                    d[3] = (uint8_t)(a / cnt);
                }
            }
            free(pixels);
            pixels = scaled;
            data_size = scaled_size;
        }
    }

    lv_img_decoder_close(&dsc);
    pthread_mutex_unlock(&g_decode_mutex);

    /* 强制设置 .bin 头: info 阶段 cf=RAW_ALPHA(2), 但像素已是原生 RGBA,
     * 必须用 TRUE_COLOR_ALPHA 内置解码器才能识别 (要求 cf 4~14) */
    lv_img_header_t real_header = {0};
    real_header.w = (lv_coord_t)tw;
    real_header.h = (lv_coord_t)th;
#if LV_COLOR_DEPTH == 32
    real_header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
#else
    real_header.cf = lv_img_cf_has_alpha(dsc.header.cf)
                     ? LV_IMG_CF_TRUE_COLOR_ALPHA
                     : LV_IMG_CF_TRUE_COLOR;
#endif

    char bin_path[256];
    snprintf(bin_path, sizeof(bin_path), "/tmp/lv_pre_%d.bin",
             (int)(intptr_t)pthread_self());
    FILE *fbin = fopen(bin_path, "wb");
    if (fbin) {
        fwrite(&real_header, sizeof(real_header), 1, fbin);
        fwrite(pixels, 1, data_size, fbin);
        fclose(fbin);
    }
    free(pixels);

    image_cache_put(path, (uint8_t *)strdup(bin_path),
                    real_header.w, real_header.h, (uint8_t)real_header.cf);
    free(path);
    return NULL;
}

/** 启动预加载 (相邻两张) */
static void start_preload(auto_screen_t *self)
{
    int prev_idx = self->cur_index - 1;
    int next_idx = self->cur_index + 1;

    for (int dir = 0; dir < 2; dir++) {
        int idx = (dir == 0) ? prev_idx : next_idx;
        if (idx < 0 || idx >= self->total_images) continue;

        char *path = strdup(self->image_list[idx]);
        if (!path) continue;

        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&tid, &attr, preload_worker, path);
        pthread_attr_destroy(&attr);
    }
}

/* ================================================================
 *  显示当前图片 (优先缓存)
 * ================================================================ */
static void show_current(auto_screen_t *self)
{
    if (self->total_images == 0 || self->cur_index < 0) {
        lv_label_set_text(self->page_label, "0 / 0");
        return;
    }

    const char *path = self->image_list[self->cur_index];
    struct timeval t1, t2;
    gettimeofday(&t1, NULL);

    /* 1. 查询预加载缓存 (存储的是已解码的 RGBA 像素) */
    uint32_t cw, ch;
    uint8_t cf;
    uint8_t *cached_px = image_cache_lookup(path, &cw, &ch, &cf);
    uint32_t img_w = 0, img_h = 0;

    if (cached_px) {
        /* 缓存命中: 加载预解码的 .bin 文件 (零格式解码, 直接读像素) */
        char fs_path[520];
        snprintf(fs_path, sizeof(fs_path), "S:%s", (char *)cached_px);
        lv_img_set_src(self->img_obj, fs_path);
        img_w = cw;
        img_h = ch;
    } else {
        /* 缓存未命中: 同步解码 */
        char fs_path[520];
        snprintf(fs_path, sizeof(fs_path), "S:%s", path);
        lv_img_set_src(self->img_obj, fs_path);

        lv_img_header_t header;
        if (lv_img_decoder_get_info(fs_path, &header) == LV_RES_OK) {
            img_w = header.w;
            img_h = header.h;
        }
    }

    /* 缩放适配: 等比缩放至图片容器内 */
    if (img_w > 0 && img_h > 0) {
        uint16_t zoom_x = (IMG_AREA_W * 256) / img_w;
        uint16_t zoom_y = (IMG_AREA_H * 256) / img_h;
        uint16_t zoom = (zoom_x < zoom_y) ? zoom_x : zoom_y;
        if (zoom > 256) zoom = 256;
        lv_img_set_zoom(self->img_obj, zoom);
    }
    lv_obj_center(self->img_obj);

    gettimeofday(&t2, NULL);
    long us = (t2.tv_sec - t1.tv_sec) * 1000000
            + (t2.tv_usec - t1.tv_usec);
    printf("[预加载] %s → %.3f ms %s\n",
           base_name(path), us / 1000.0,
           cached_px ? "(缓存命中)" : "(同步解码)");

    /* 更新 UI */
    lv_label_set_text(self->title_label, base_name(path));
    char buf[32];
    snprintf(buf, sizeof(buf), "%d / %d", self->cur_index + 1, self->total_images);
    lv_label_set_text(self->page_label, buf);

    lv_obj_set_style_bg_opa(self->btn_prev,
        (self->cur_index > 0) ? LV_OPA_COVER : LV_OPA_50, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(self->btn_next,
        (self->cur_index < self->total_images - 1) ? LV_OPA_COVER : LV_OPA_50, LV_PART_MAIN);

    /* 2. 启动后台预加载 */
    start_preload(self);
}

/* ---- 事件 ---- */
static void on_back_click(lv_event_t *e)
{ (void)e; page_manager_navigate("manual", NULL); }

static void on_prev_click(lv_event_t *e)
{ auto_screen_t *s = lv_event_get_user_data(e); if (s->cur_index > 0) { s->cur_index--; show_current(s); } }

static void on_next_click(lv_event_t *e)
{ auto_screen_t *s = lv_event_get_user_data(e); if (s->cur_index < s->total_images - 1) { s->cur_index++; show_current(s); } }

/* ---- 公开: 设置图片列表 ---- */
void auto_screen_open_images(auto_screen_t *self,
                              const char (*list)[512],
                              int count, int focus_idx)
{
    self->total_images = (count > MAX_IMAGES) ? MAX_IMAGES : count;
    for (int i = 0; i < self->total_images; i++)
        snprintf(self->image_list[i], sizeof(self->image_list[0]), "%s", list[i]);
    self->cur_index = (focus_idx >= 0 && focus_idx < self->total_images) ? focus_idx : 0;
}

/* ---- 构造/析构 ---- */
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

    /* screen */
    self->base.screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(self->base.screen, lv_color_hex(0xE7DBB5), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->base.screen, 0, LV_PART_MAIN);
    lv_obj_clear_flag(self->base.screen, LV_OBJ_FLAG_SCROLLABLE);

    /* 标题栏 */
    self->title_bar = lv_obj_create(self->base.screen);
    lv_obj_set_style_bg_color(self->title_bar, lv_color_hex(0xD4C4A0), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->title_bar, 0, LV_PART_MAIN);
    lv_obj_clear_flag(self->title_bar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(self->title_bar, 0, 0);
    lv_obj_set_size(self->title_bar, SCREEN_W, TITLE_BAR_H);

    self->back_btn = lv_btn_create(self->title_bar);
    lv_obj_set_style_bg_color(self->back_btn, lv_color_hex(0xC4B490), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->back_btn, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(self->back_btn, BACK_BTN_R, LV_PART_MAIN);
    lv_obj_set_size(self->back_btn, BACK_BTN_W, BACK_BTN_H);
    lv_obj_set_pos(self->back_btn, BACK_BTN_X, BACK_BTN_Y);
    lv_obj_add_event_cb(self->back_btn, on_back_click, LV_EVENT_CLICKED, NULL);
    lv_obj_t *bl = lv_label_create(self->back_btn);
    lv_label_set_text(bl, "← 返回");
    lv_obj_set_style_text_color(bl, lv_color_hex(0x5A4A3A), LV_PART_MAIN);
    if (font_cjk_16) lv_obj_set_style_text_font(bl, font_cjk_16, LV_PART_MAIN);
    lv_obj_center(bl);

    self->title_label = lv_label_create(self->title_bar);
    lv_obj_set_style_text_color(self->title_label, lv_color_hex(0x5A4A3A), LV_PART_MAIN);
    if (font_cjk_24) lv_obj_set_style_text_font(self->title_label, font_cjk_24, LV_PART_MAIN);
    lv_obj_set_pos(self->title_label, TITLE_LABEL_X, TITLE_LABEL_Y);

    /* 图片容器 — 提供背景与圆角裁剪 */
    self->img_container = lv_obj_create(self->base.screen);
    lv_obj_set_pos(self->img_container, IMG_AREA_X, IMG_AREA_Y);
    lv_obj_set_size(self->img_container, IMG_AREA_W, IMG_AREA_H);
    lv_obj_set_style_bg_color(self->img_container, lv_color_hex(0x3D3222), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(self->img_container, LV_OPA_30, LV_PART_MAIN);
    lv_obj_set_style_radius(self->img_container, IMG_RADIUS, LV_PART_MAIN);
    lv_obj_set_style_border_width(self->img_container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(self->img_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_clip_corner(self->img_container, true, LV_PART_MAIN);

    /* 图片 — 作为容器子对象, REAL + SIZE_CONTENT 使控件贴合图像，不再平铺 */
    self->img_obj = lv_img_create(self->img_container);
    lv_img_set_size_mode(self->img_obj, LV_IMG_SIZE_MODE_REAL);
    lv_obj_set_size(self->img_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(self->img_obj);

    /* 底栏 */
    self->bottom_bar = lv_obj_create(self->base.screen);
    lv_obj_set_style_bg_color(self->bottom_bar, lv_color_hex(0xD4C4A0), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->bottom_bar, 0, LV_PART_MAIN);
    lv_obj_clear_flag(self->bottom_bar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(self->bottom_bar, 0, BOTTOM_BAR_Y);
    lv_obj_set_size(self->bottom_bar, SCREEN_W, BOTTOM_BAR_H);

    /* 上一张 */
    self->btn_prev = lv_btn_create(self->bottom_bar);
    lv_obj_set_style_bg_color(self->btn_prev, lv_color_hex(0xC4B490), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->btn_prev, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(self->btn_prev, NAV_BTN_R, LV_PART_MAIN);
    lv_obj_set_size(self->btn_prev, NAV_BTN_W, NAV_BTN_H);
    lv_obj_set_pos(self->btn_prev, BTN_PREV_X, NAV_BTN_Y);
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
    lv_obj_set_style_radius(self->btn_next, NAV_BTN_R, LV_PART_MAIN);
    lv_obj_set_size(self->btn_next, NAV_BTN_W, NAV_BTN_H);
    lv_obj_set_pos(self->btn_next, BTN_NEXT_X, NAV_BTN_Y);
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
    lv_obj_set_pos(self->page_label, PAGE_LABEL_X, PAGE_LABEL_Y);

    show_current(self);
    lv_scr_load(self->base.screen);
    return self->base.screen;
}

static void auto_screen_destroy(base_screen_t *base)
{ auto_screen_t *s = (auto_screen_t *)base; if (s->base.screen) { lv_obj_del(s->base.screen); s->base.screen = NULL; } }
