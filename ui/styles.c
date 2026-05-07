#include "styles.h"
#include <stdlib.h>

lv_style_t *style_bg_dark   = NULL;
lv_style_t *style_text_white = NULL;

void styles_init(void)
{
    style_bg_dark = malloc(sizeof(lv_style_t));
    lv_style_init(style_bg_dark);
    lv_style_set_bg_color(style_bg_dark, lv_color_hex(0x202020));

    style_text_white = malloc(sizeof(lv_style_t));
    lv_style_init(style_text_white);
    lv_style_set_text_color(style_text_white, lv_color_hex(0xFFFFFF));
}
