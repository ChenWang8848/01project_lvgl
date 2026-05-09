/**
 * @file    styles.c
 * @brief   全局样式实现
 *
 * 在应用启动时调用 styles_init() 创建所有全局 style 对象。
 * 所有页面通过 extern 引用这些 style 句柄。
 */

#include "styles.h"
#include <stdlib.h>

/* ---- 基础 ---- */
lv_style_t *style_bg_dark    = NULL;
lv_style_t *style_text_white = NULL;

/* ---- 主界面图标 ---- */
lv_style_t *style_icon_blue    = NULL;
lv_style_t *style_icon_green   = NULL;
lv_style_t *style_icon_orange  = NULL;
lv_style_t *style_icon_text    = NULL;

/* ---- 子页面标题栏 ---- */
lv_style_t *style_title_bar  = NULL;
lv_style_t *style_title_text = NULL;
lv_style_t *style_back_btn   = NULL;

/* ---- 辅助宏: 分配 + 初始化 style ---- */
#define ALLOC_STYLE(name) do { \
    name = malloc(sizeof(lv_style_t)); \
    lv_style_init(name); \
} while (0)

void styles_init(void)
{
    /* ========== 基础样式 ========== */
    ALLOC_STYLE(style_bg_dark);
    lv_style_set_bg_color(style_bg_dark, lv_color_hex(0x202020));

    ALLOC_STYLE(style_text_white);
    lv_style_set_text_color(style_text_white, lv_color_hex(0xFFFFFF));

    /* ========== 主界面图标背景 (90x90 圆角方块) ========== */

    /* 蓝色 — 手动播放 */
    ALLOC_STYLE(style_icon_blue);
    lv_style_set_bg_color(style_icon_blue, lv_color_hex(0x2196F3));
    lv_style_set_bg_opa(style_icon_blue, LV_OPA_COVER);
    lv_style_set_radius(style_icon_blue, 18);
    lv_style_set_width(style_icon_blue, 90);
    lv_style_set_height(style_icon_blue, 90);

    /* 绿色 — 自动播放 */
    ALLOC_STYLE(style_icon_green);
    lv_style_set_bg_color(style_icon_green, lv_color_hex(0x4CAF50));
    lv_style_set_bg_opa(style_icon_green, LV_OPA_COVER);
    lv_style_set_radius(style_icon_green, 18);
    lv_style_set_width(style_icon_green, 90);
    lv_style_set_height(style_icon_green, 90);

    /* 橙色 — 设置 */
    ALLOC_STYLE(style_icon_orange);
    lv_style_set_bg_color(style_icon_orange, lv_color_hex(0xFF9800));
    lv_style_set_bg_opa(style_icon_orange, LV_OPA_COVER);
    lv_style_set_radius(style_icon_orange, 18);
    lv_style_set_width(style_icon_orange, 90);
    lv_style_set_height(style_icon_orange, 90);

    /* 图标内白色大字 (28pt, 居中) */
    ALLOC_STYLE(style_icon_text);
    lv_style_set_text_color(style_icon_text, lv_color_hex(0xFFFFFF));
    lv_style_set_text_font(style_icon_text, &lv_font_montserrat_28);
    lv_style_set_text_align(style_icon_text, LV_TEXT_ALIGN_CENTER);

    /* ========== 子页面标题栏 ========== */

    /* 标题栏: 顶部 56px, 半透明深色 */
    ALLOC_STYLE(style_title_bar);
    lv_style_set_bg_color(style_title_bar, lv_color_hex(0x303030));
    lv_style_set_bg_opa(style_title_bar, LV_OPA_COVER);
    lv_style_set_width(style_title_bar, 800);
    lv_style_set_height(style_title_bar, 56);

    /* 标题文字: 白色 18pt */
    ALLOC_STYLE(style_title_text);
    lv_style_set_text_color(style_title_text, lv_color_hex(0xFFFFFF));
    lv_style_set_text_font(style_title_text, &lv_font_montserrat_28);

    /* 返回按钮: 浅色背景 + 圆角 */
    ALLOC_STYLE(style_back_btn);
    lv_style_set_bg_color(style_back_btn, lv_color_hex(0x505050));
    lv_style_set_bg_opa(style_back_btn, LV_OPA_COVER);
    lv_style_set_radius(style_back_btn, 10);
    lv_style_set_width(style_back_btn, 80);
    lv_style_set_height(style_back_btn, 40);
}
