/**
 * @file    mouse.c
 * @brief   USB 鼠标驱动实现
 *
 * 当前为 Phase 2 桩代码。Phase 3 将实现:
 *   1. 打开 /dev/input/eventX 设备
 *   2. 读取 EV_REL (相对位移) 和 EV_KEY (按键) 事件
 *   3. 累积坐标增量，传递 lv_indev_data_t 给 LVGL
 */

#include "mouse.h"
#include <string.h>

static int  mouse_init_fn(input_driver_t *base);
static bool mouse_read(input_driver_t *base, lv_indev_drv_t *drv, lv_indev_data_t *data);
static void mouse_deinit(input_driver_t *base);

void mouse_init(mouse_t *self, const char *dev_path)
{
    (void)dev_path;
    memset(self, 0, sizeof(*self));
    self->base.name   = "mouse";
    self->base.init   = mouse_init_fn;
    self->base.read   = mouse_read;
    self->base.deinit = mouse_deinit;
}

static int mouse_init_fn(input_driver_t *base)
{
    (void)base;
    return 0; /* TODO: Phase 3 */
}

static bool mouse_read(input_driver_t *base, lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    (void)base;
    (void)drv;
    (void)data;
    return false; /* TODO: Phase 3 */
}

static void mouse_deinit(input_driver_t *base)
{
    (void)base;
}
