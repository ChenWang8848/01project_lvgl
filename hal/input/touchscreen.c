#include "touchscreen.h"
#include <string.h>

static int  ts_init(input_driver_t *base);
static bool ts_read(input_driver_t *base, lv_indev_drv_t *drv, lv_indev_data_t *data);
static void ts_deinit(input_driver_t *base);

void touchscreen_init(touchscreen_t *self, const char *dev_path)
{
    (void)dev_path;
    memset(self, 0, sizeof(*self));
    self->base.name   = "touchscreen";
    self->base.init   = ts_init;
    self->base.read   = ts_read;
    self->base.deinit = ts_deinit;
}

static int ts_init(input_driver_t *base)
{
    (void)base;
    return 0; /* stub: TODO Phase 3 */
}

static bool ts_read(input_driver_t *base, lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    (void)base;
    (void)drv;
    (void)data;
    return false;
}

static void ts_deinit(input_driver_t *base)
{
    (void)base;
}
