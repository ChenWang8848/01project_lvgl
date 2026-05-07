/**
 * @file    lvgl_input.c
 * @brief   LVGL 输入适配层实现
 *
 * 当前为 Phase 2 桩代码。Phase 3 将实现:
 *   1. 创建 lv_indev_drv_t 结构体
 *   2. 绑定 input_driver_t->read() 为 indev read 回调
 *   3. 调用 lv_indev_drv_register() 注册到 LVGL
 */

#include "lvgl_input.h"

int lvgl_input_register(input_driver_t *drv, lv_indev_type_t type)
{
    (void)drv;
    (void)type;
    return 0; /* TODO: Phase 3 */
}

void lvgl_input_init_all(void)
{
    /* TODO: Phase 3 — 注册触摸屏和鼠标到 LVGL */
}
