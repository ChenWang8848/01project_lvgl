/**
 * @file    mouse.h
 * @brief   USB 鼠标驱动 — input_driver_t 的具体实现 (evdev)
 *
 * Phase 3 将实现完整的鼠标读取逻辑。
 */

#ifndef MOUSE_H
#define MOUSE_H

#include "input_driver.h"

typedef struct {
    input_driver_t base;        /**< 父类 (vtable) */
    int fd;                     /**< 鼠标设备文件描述符 */
} mouse_t;

void mouse_init(mouse_t *self, const char *dev_path);

#endif
