/**
 * @file    lvgl_input.h
 * @brief   LVGL 输入适配层 — 将 input_driver_t 注册为 LVGL indev (输入设备)
 *
 * Phase 3 将实现完整的输入设备注册逻辑。
 */

#ifndef LVGL_INPUT_H
#define LVGL_INPUT_H

#include "lvgl/lvgl.h"
#include "input_driver.h"

/**
 * @brief 将输入驱动注册为 LVGL 输入设备
 * @param drv  输入驱动对象 (已初始化)
 * @param type LVGL 输入设备类型 (LV_INDEV_TYPE_POINTER / TOUCHPAD 等)
 * @return 0: 成功
 */
int  lvgl_input_register(input_driver_t *drv, lv_indev_type_t type);

/**
 * @brief 初始化所有输入设备 (触摸屏 + 鼠标)
 */
void lvgl_input_init_all(void);

#endif
