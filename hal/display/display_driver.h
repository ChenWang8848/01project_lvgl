#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include "lvgl/lvgl.h"

typedef struct display_driver_s display_driver_t;

struct display_driver_s {
    const char *name;
    int  (*init)(display_driver_t *self);
    void (*get_resolution)(display_driver_t *self, int *w, int *h);
    int  (*get_bpp)(display_driver_t *self);
    void (*flush)(display_driver_t *self, lv_disp_drv_t *drv,
                  const lv_area_t *area, lv_color_t *color_p);
    void (*wait_cb)(display_driver_t *self, lv_disp_drv_t *drv);
    void (*deinit)(display_driver_t *self);
};

#endif
