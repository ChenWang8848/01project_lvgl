#ifndef LVGL_DISPLAY_H
#define LVGL_DISPLAY_H

#include "lvgl/lvgl.h"
#include "display_driver.h"

typedef struct {
    lv_disp_drv_t      drv;
    lv_disp_draw_buf_t draw_buf;
    lv_color_t        *buf1;
    lv_color_t        *buf2;          /* NULL = single buffer */
    display_driver_t  *hw;
    int                hor_res;
    int                ver_res;
} lvgl_display_t;

int  lvgl_display_init(lvgl_display_t *self, display_driver_t *hw);
void lvgl_display_deinit(lvgl_display_t *self);

#endif
