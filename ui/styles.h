/**
 * @file    styles.h
 * @brief   全局样式/主题定义 — 为所有页面提供统一的视觉风格
 *
 * 使用 LVGL style 对象定义颜色、字体、间距等视觉属性。
 * 页面通过引用这些全局 style 句柄实现统一的 UI 风格。
 *
 * 后续可扩展: 深色/浅色主题切换、字体大小预设、间距体系。
 */

#ifndef STYLES_H
#define STYLES_H

#include "lvgl/lvgl.h"

/** @brief 初始化全局样式 (创建 style 对象, 设置属性) */
void styles_init(void);

/** @brief 深色背景样式 (20-20-20) */
extern lv_style_t *style_bg_dark;

/** @brief 白色文字样式 */
extern lv_style_t *style_text_white;

#endif
