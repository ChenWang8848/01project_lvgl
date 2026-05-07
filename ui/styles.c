/**
 * @file    styles.c
 * @brief   全局样式实现
 *
 * 在应用启动时调用 styles_init() 创建全局 style 对象。
 * 所有页面通过 extern 引用这些 style 句柄，无需各自创建。
 */

#include "styles.h"
#include <stdlib.h>

lv_style_t *style_bg_dark    = NULL;
lv_style_t *style_text_white = NULL;

void styles_init(void)
{
    /* 深色背景 */
    style_bg_dark = malloc(sizeof(lv_style_t));
    lv_style_init(style_bg_dark);
    lv_style_set_bg_color(style_bg_dark, lv_color_hex(0x202020));

    /* 白色文字 */
    style_text_white = malloc(sizeof(lv_style_t));
    lv_style_init(style_text_white);
    lv_style_set_text_color(style_text_white, lv_color_hex(0xFFFFFF));
}
