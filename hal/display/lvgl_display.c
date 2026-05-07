#include "lvgl_display.h"
#include <stdlib.h>
#include <string.h>

static void disp_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area,
                          lv_color_t *color_p)
{
    lvgl_display_t *self = (lvgl_display_t *)drv->user_data;
    if (self->hw && self->hw->flush)
        self->hw->flush(self->hw, drv, area, color_p);
    else
        lv_disp_flush_ready(drv);
}

static void disp_wait_cb(lv_disp_drv_t *drv)
{
    lvgl_display_t *self = (lvgl_display_t *)drv->user_data;
    if (self->hw && self->hw->wait_cb)
        self->hw->wait_cb(self->hw, drv);
}

int lvgl_display_init(lvgl_display_t *self, display_driver_t *hw)
{
    memset(self, 0, sizeof(*self));
    self->hw = hw;

    /* get resolution from hw */
    int w = 800, h = 480;
    if (hw && hw->get_resolution)
        hw->get_resolution(hw, &w, &h);
    self->hor_res = w;
    self->ver_res = h;

    /* allocate draw buffer */
    uint32_t buf_size = w * h;
    self->buf1 = (lv_color_t *)malloc(buf_size * sizeof(lv_color_t));
    if (!self->buf1)
        return -1;

    lv_disp_draw_buf_init(&self->draw_buf, self->buf1, self->buf2, buf_size);

    lv_disp_drv_init(&self->drv);
    self->drv.draw_buf   = &self->draw_buf;
    self->drv.flush_cb   = disp_flush_cb;
    self->drv.wait_cb    = disp_wait_cb;
    self->drv.hor_res    = w;
    self->drv.ver_res    = h;
    self->drv.user_data  = self;

    lv_disp_t *disp = lv_disp_drv_register(&self->drv);
    (void)disp;

    return 0;
}

void lvgl_display_deinit(lvgl_display_t *self)
{
    if (self->buf1) {
        free(self->buf1);
        self->buf1 = NULL;
    }
}
