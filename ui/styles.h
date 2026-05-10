/**
 * @file    styles.h
 * @brief   全局样式/主题 — 米黄色阅读主题 (0xE7DBB5)
 */
#ifndef STYLES_H
#define STYLES_H
#include "lvgl/lvgl.h"

void styles_init(void);

/* ---- FreeType 中文字体 ---- */
extern lv_font_t *font_cjk_24;
extern lv_font_t *font_cjk_16;

/* ---- 基础 ---- */
extern lv_style_t *style_bg_beige;     /**< 米黄背景 0xE7DBB5 */
extern lv_style_t *style_text_dark;    /**< 深色文字 0x5A4A3A */

/* ---- 主界面图标 ---- */
extern lv_style_t *style_icon_blue;
extern lv_style_t *style_icon_green;
extern lv_style_t *style_icon_orange;
extern lv_style_t *style_icon_text;    /**< 图标内符号 (白色居中) */
extern lv_style_t *style_label_cn;     /**< 图标下方中文标签 (深色居中) */

/* ---- 子页面标题栏 ---- */
extern lv_style_t *style_title_bar;    /**< 标题栏: 深米黄 */
extern lv_style_t *style_title_text;   /**< 标题文字: 深色 */
extern lv_style_t *style_back_btn;     /**< 返回按钮 */
extern lv_style_t *style_back_label;   /**< 返回按钮文字 */

/* ---- 无边框/无滚动辅助样式 ---- */
extern lv_style_t *style_no_scroll;    /**< 禁用滚动 + 无边框 */

#endif
