/**
 * @file    touchscreen.c
 * @brief   触摸屏驱动实现
 *
 * 当前为 Phase 2 桩代码。Phase 3 将实现:
 *   1. 打开 tslib/evdev 设备
 *   2. 读取触摸坐标 (X/Y/压力)
 *   3. 坐标校准与滤波
 *   4. 传递 lv_indev_data_t 给 LVGL
 */

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
    return 0; /* TODO: Phase 3 — 打开触摸屏设备 */
}

static bool ts_read(input_driver_t *base, lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    (void)base;
    (void)drv;
    (void)data;
    return false; /* TODO: Phase 3 — 读取触摸数据 */
}

static void ts_deinit(input_driver_t *base)
{
    (void)base;
}
