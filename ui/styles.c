/**
 * @file    styles.c
 * @brief   全局样式实现 — 米黄色阅读主题 (0xE7DBB5)
 */

#include "styles.h"
#include "config/config.h"
#include <stdio.h>
#include <stdlib.h>

#if LV_USE_FREETYPE
#include "lvgl/src/extra/libs/freetype/lv_freetype.h"
#endif

/* ---- FreeType ---- */
lv_font_t *font_cjk_24 = NULL;
lv_font_t *font_cjk_16 = NULL;

/* ---- 基础 ---- */
lv_style_t *style_bg_beige   = NULL;
lv_style_t *style_text_dark  = NULL;

/* ---- 主界面图标 ---- */
lv_style_t *style_icon_blue    = NULL;
lv_style_t *style_icon_green   = NULL;
lv_style_t *style_icon_orange  = NULL;
lv_style_t *style_icon_text    = NULL;
lv_style_t *style_label_cn     = NULL;

/* ---- 子页面标题栏 ---- */
lv_style_t *style_title_bar  = NULL;
lv_style_t *style_title_text = NULL;
lv_style_t *style_back_btn   = NULL;
lv_style_t *style_back_label = NULL;

/* ---- 无边框/无滚动 ---- */
lv_style_t *style_no_scroll = NULL;

#define ALLOC_STYLE(name) do { \
    name = malloc(sizeof(lv_style_t)); \
    lv_style_init(name); \
} while (0)

static lv_font_t *create_freetype_font(const char *path, uint16_t weight)
{
#if LV_USE_FREETYPE
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        printf("[ERROR] 字体文件不存在: %s\n", path);
        printf("[INFO]  请将中文字体文件放到该路径,"
               "或修改 config/config.h 中的 FREETYPE_FONT_PATH\n");
        return NULL;
    }
    fclose(fp);

    lv_ft_info_t *info = malloc(sizeof(lv_ft_info_t));
    memset(info, 0, sizeof(*info));
    info->name   = path;
    info->weight = weight;
    info->style  = FT_FONT_STYLE_NORMAL;

    if (!lv_ft_font_init(info)) {
        printf("[ERROR] FreeType 字体加载失败: %s (size=%d)\n", path, weight);
        free(info);
        return NULL;
    }
    printf("[INFO]  FreeType 字体加载成功: %s (size=%d)\n", path, weight);
    return info->font;
#else
    (void)path; (void)weight;
    return NULL;
#endif
}

void styles_init(void)
{
    /* ================================================================
     *  FreeType
     * ================================================================ */
#if LV_USE_FREETYPE
    lv_freetype_init(2, 2, 128 * 1024);
    font_cjk_24 = create_freetype_font(FREETYPE_FONT_PATH, 24);
    if (!font_cjk_24) font_cjk_24 = (lv_font_t *)&lv_font_montserrat_28;
    font_cjk_16 = create_freetype_font(FREETYPE_FONT_PATH, 16);
    if (!font_cjk_16) font_cjk_16 = (lv_font_t *)&lv_font_montserrat_14;
#endif

    /* ================================================================
     *  基础样式 (米黄主题)
     * ================================================================ */
    ALLOC_STYLE(style_bg_beige);
    lv_style_set_bg_color(style_bg_beige, lv_color_hex(0xE7DBB5));

    ALLOC_STYLE(style_text_dark);
    lv_style_set_text_color(style_text_dark, lv_color_hex(0x5A4A3A));

    /* ================================================================
     *  主界面图标背景 (90x90, 圆角18)
     * ================================================================ */
    ALLOC_STYLE(style_icon_blue);
    lv_style_set_bg_color(style_icon_blue, lv_color_hex(0x2196F3));
    lv_style_set_bg_opa(style_icon_blue, LV_OPA_COVER);
    lv_style_set_radius(style_icon_blue, 18);
    lv_style_set_width(style_icon_blue, 90);
    lv_style_set_height(style_icon_blue, 90);
    lv_style_set_border_width(style_icon_blue, 0);

    ALLOC_STYLE(style_icon_green);
    lv_style_set_bg_color(style_icon_green, lv_color_hex(0x4CAF50));
    lv_style_set_bg_opa(style_icon_green, LV_OPA_COVER);
    lv_style_set_radius(style_icon_green, 18);
    lv_style_set_width(style_icon_green, 90);
    lv_style_set_height(style_icon_green, 90);
    lv_style_set_border_width(style_icon_green, 0);

    ALLOC_STYLE(style_icon_orange);
    lv_style_set_bg_color(style_icon_orange, lv_color_hex(0xFF9800));
    lv_style_set_bg_opa(style_icon_orange, LV_OPA_COVER);
    lv_style_set_radius(style_icon_orange, 18);
    lv_style_set_width(style_icon_orange, 90);
    lv_style_set_height(style_icon_orange, 90);
    lv_style_set_border_width(style_icon_orange, 0);

    /* 图标内符号 (白色, Montserrat 28pt, 居中) */
    ALLOC_STYLE(style_icon_text);
    lv_style_set_text_color(style_icon_text, lv_color_hex(0xFFFFFF));
    lv_style_set_text_font(style_icon_text, &lv_font_montserrat_28);
    lv_style_set_text_align(style_icon_text, LV_TEXT_ALIGN_CENTER);

    /* 图标下方中文标签 (深色, FreeType 16pt, 居中) */
    ALLOC_STYLE(style_label_cn);
    lv_style_set_text_color(style_label_cn, lv_color_hex(0x5A4A3A));
    if (font_cjk_16)
        lv_style_set_text_font(style_label_cn, font_cjk_16);
    lv_style_set_text_align(style_label_cn, LV_TEXT_ALIGN_CENTER);

    /* ================================================================
     *  子页面标题栏 (深米黄 0xD4C4A0)
     * ================================================================ */
    ALLOC_STYLE(style_title_bar);
    lv_style_set_bg_color(style_title_bar, lv_color_hex(0xD4C4A0));
    lv_style_set_bg_opa(style_title_bar, LV_OPA_COVER);
    lv_style_set_border_width(style_title_bar, 0);
    lv_style_set_width(style_title_bar, 800);
    lv_style_set_height(style_title_bar, 56);

    /* 标题文字 (深色, FreeType 24pt) */
    ALLOC_STYLE(style_title_text);
    lv_style_set_text_color(style_title_text, lv_color_hex(0x5A4A3A));
    if (font_cjk_24)
        lv_style_set_text_font(style_title_text, font_cjk_24);

    /* 返回按钮 */
    ALLOC_STYLE(style_back_btn);
    lv_style_set_bg_color(style_back_btn, lv_color_hex(0xC4B490));
    lv_style_set_bg_opa(style_back_btn, LV_OPA_COVER);
    lv_style_set_border_width(style_back_btn, 0);
    lv_style_set_radius(style_back_btn, 10);
    lv_style_set_width(style_back_btn, 90);
    lv_style_set_height(style_back_btn, 40);

    /* 返回按钮文字 (深色, FreeType 16pt) */
    ALLOC_STYLE(style_back_label);
    lv_style_set_text_color(style_back_label, lv_color_hex(0x5A4A3A));
    if (font_cjk_16)
        lv_style_set_text_font(style_back_label, font_cjk_16);

    /* ================================================================
     *  无边框 + 无滚动 (用于容器和 screen)
     * ================================================================ */
    ALLOC_STYLE(style_no_scroll);
    lv_style_set_border_width(style_no_scroll, 0);
    lv_style_set_pad_all(style_no_scroll, 0);
}
