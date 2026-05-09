/**
 * @file    touchscreen.c
 * @brief   触摸屏驱动 — evdev 实现
 *
 * 工作流程:
 *   1. open()  /dev/input/eventX (非阻塞)
 *   2. read() 循环读取 struct input_event
 *   3. 解析 EV_ABS (ABS_X/ABS_Y) 获取坐标
 *   4. 解析 EV_KEY (BTN_TOUCH) 获取按下/释放
 *   5. 将数据填入 lv_indev_data_t 返回给 LVGL
 */

#include "touchscreen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

/* ================================================================
 *  vtable 实现
 * ================================================================ */
static int  ts_init(input_driver_t *base);
static bool ts_read(input_driver_t *base, lv_indev_drv_t *drv, lv_indev_data_t *data);
static void ts_deinit(input_driver_t *base);

/* ================================================================
 *  构造
 * ================================================================ */

void touchscreen_init(touchscreen_t *self, const char *dev_path)
{
    memset(self, 0, sizeof(*self));
    self->base.name        = "touchscreen";
    self->base.init        = ts_init;
    self->base.read        = ts_read;
    self->base.deinit      = ts_deinit;
    self->fd               = -1;
    self->dev_path         = dev_path ? dev_path : "/dev/input/event2";
    self->last_pressed     = LV_INDEV_STATE_REL;
}

/* ================================================================
 *  设备初始化
 * ================================================================ */

static int ts_init(input_driver_t *base)
{
    touchscreen_t *self = (touchscreen_t *)base;

    self->fd = open(self->dev_path, O_RDONLY | O_NONBLOCK);
    if (self->fd < 0) {
        perror("touchscreen: open");
        return -1;
    }

    printf("touchscreen: opened %s\n", self->dev_path);
    return 0;
}

/* ================================================================
 *  读取触摸数据 — LVGL indev read_cb
 * ================================================================ */

static bool ts_read(input_driver_t *base, lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    touchscreen_t *self = (touchscreen_t *)base;
    struct input_event in;
    bool got_data = false;

    /* 读取所有待处理事件 */
    while (read(self->fd, &in, sizeof(struct input_event)) > 0) {
        if (in.type == EV_ABS) {
            if (in.code == ABS_X || in.code == ABS_MT_POSITION_X) {
                self->last_x = in.value;
                got_data = true;
            } else if (in.code == ABS_Y || in.code == ABS_MT_POSITION_Y) {
                self->last_y = in.value;
                got_data = true;
            }
        } else if (in.type == EV_KEY) {
            if (in.code == BTN_TOUCH) {
                self->last_pressed = (in.value > 0)
                    ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
                got_data = true;
            }
        }
    }

    /* 填充 LVGL 数据 */
    data->point.x = self->last_x;
    data->point.y = self->last_y;
    data->state   = self->last_pressed;

    /* 边界裁剪到屏幕范围内 */
    lv_disp_t *disp = drv->disp ? drv->disp : lv_disp_get_default();
    if (disp) {
        int hor_res = lv_disp_get_hor_res(disp);
        int ver_res = lv_disp_get_ver_res(disp);
        if (data->point.x < 0)           data->point.x = 0;
        if (data->point.y < 0)           data->point.y = 0;
        if (data->point.x >= hor_res)    data->point.x = hor_res - 1;
        if (data->point.y >= ver_res)    data->point.y = ver_res - 1;
    }

    return got_data;
}

/* ================================================================
 *  清理
 * ================================================================ */

static void ts_deinit(input_driver_t *base)
{
    touchscreen_t *self = (touchscreen_t *)base;
    if (self->fd >= 0) {
        close(self->fd);
        self->fd = -1;
    }
}
