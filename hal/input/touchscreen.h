/**
 * @file    touchscreen.h
 * @brief   触摸屏驱动 — input_driver_t 的具体实现 (tslib / evdev)
 *
 * Phase 3 将实现完整的触摸屏读取逻辑。
 * 当前为桩代码 (stub)，返回无数据。
 */

#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include "input_driver.h"

typedef struct {
    input_driver_t base;        /**< 父类 (vtable) */
    int fd;                     /**< 触摸屏设备文件描述符 */
} touchscreen_t;

/**
 * @brief 构造触摸屏驱动对象
 * @param self     对象指针
 * @param dev_path 设备路径 (如 "/dev/input/event2")
 */
void touchscreen_init(touchscreen_t *self, const char *dev_path);

#endif
