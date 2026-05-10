/**
 * @file    styles.h
 * @brief   全局样式/主题定义 — 为所有页面提供统一的视觉风格
 *
 * 支持 FreeType 中文字体渲染。
 * 字体路径通过 config.h 中的 FREETYPE_FONT_PATH 配置。
 */

#ifndef STYLES_H
#define STYLES_H

#include "lvgl/lvgl.h"

void styles_init(void);

/* ---- FreeType 中文字体句柄 ---- */
extern lv_font_t *font_cjk_24;    /**< 中文字体 24pt (标题) */
extern lv_font_t *font_cjk_16;    /**< 中文字体 16pt (正文) */

/* ---- 基础样式 ---- */
extern lv_style_t *style_bg_dark;
extern lv_style_t *style_text_white;

/* ---- 主界面图标样式 ---- */
extern lv_style_t *style_icon_blue;
extern lv_style_t *style_icon_green;
extern lv_style_t *style_icon_orange;
extern lv_style_t *style_icon_text;   /**< 图标内符号 (Montserrat 28) */
extern lv_style_t *style_label_cn;    /**< 图标下方中文标签 (FreeType 16) */

/* ---- 子页面标题栏样式 ---- */
extern lv_style_t *style_title_bar;
extern lv_style_t *style_title_text;  /**< 标题文字 (FreeType 24) */
extern lv_style_t *style_back_btn;
extern lv_style_t *style_back_label;  /**< 返回按钮文字 (FreeType 16) */

#endif
