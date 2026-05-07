#ifndef STYLES_H
#define STYLES_H

#include "lvgl/lvgl.h"

void styles_init(void);

/* shared style handles (may be NULL if not initialized) */
extern lv_style_t *style_bg_dark;
extern lv_style_t *style_text_white;

#endif
