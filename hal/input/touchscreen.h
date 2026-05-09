/**
 * @file    touchscreen.h
 * @brief   触摸屏驱动 — evdev 实现 (input_driver_t 子类)
 */

#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include "input_driver.h"

typedef struct {
    input_driver_t base;        /**< 父类 (vtable) */
    int         fd;             /**< 触摸屏设备文件描述符 */
    const char *dev_path;       /**< 设备路径 */
    int         last_x;         /**< 当前 X 坐标 */
    int         last_y;         /**< 当前 Y 坐标 */
    int         last_pressed;   /**< 当前按下状态 (LV_INDEV_STATE_PR / REL) */
} touchscreen_t;

/**
 * @brief 构造触摸屏驱动对象
 * @param self     对象指针
 * @param dev_path 设备路径 (如 "/dev/input/event2")
 */
void touchscreen_init(touchscreen_t *self, const char *dev_path);

#endif
