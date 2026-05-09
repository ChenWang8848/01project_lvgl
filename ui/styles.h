/**
 * @file    styles.h
 * @brief   全局样式/主题定义 — 为所有页面提供统一的视觉风格
 *
 * 使用 LVGL style 对象定义颜色、字体、间距等视觉属性。
 * 页面通过引用这些全局 style 句柄实现统一的 UI 风格。
 */

#ifndef STYLES_H
#define STYLES_H

#include "lvgl/lvgl.h"

void styles_init(void);

/* ---- 基础样式 ---- */
extern lv_style_t *style_bg_dark;       /**< 深色背景 */
extern lv_style_t *style_text_white;    /**< 白色文字 */

/* ---- 主界面图标样式 ---- */
extern lv_style_t *style_icon_blue;     /**< 蓝色图标背景 (手动播放) */
extern lv_style_t *style_icon_green;    /**< 绿色图标背景 (自动播放) */
extern lv_style_t *style_icon_orange;   /**< 橙色图标背景 (设置) */
extern lv_style_t *style_icon_text;     /**< 图标内白色大字 (28pt) */

/* ---- 子页面标题栏样式 ---- */
extern lv_style_t *style_title_bar;     /**< 标题栏背景 */
extern lv_style_t *style_title_text;    /**< 标题文字 */
extern lv_style_t *style_back_btn;      /**< 返回按钮 */

#endif
