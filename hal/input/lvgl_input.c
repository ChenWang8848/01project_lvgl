/**
 * @file    lvgl_input.c
 * @brief   LVGL 输入适配层 — 将 input_driver_t 注册为 LVGL 输入设备
 */

#include "lvgl_input.h"
#include "touchscreen.h"

/* ---- 全局单例 (应用生命周期内存在) ---- */
static touchscreen_t   g_touchscreen;
static lv_indev_drv_t  g_indev_drv;

/**
 * @brief LVGL indev read 回调 — 转发到 input_driver->read()
 */
static void indev_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    input_driver_t *input = (input_driver_t *)drv->user_data;
    if (!input || !input->read) {
        data->state = LV_INDEV_STATE_REL;
        return;
    }
    input->read(input, drv, data);
}

/**
 * @brief 将 input_driver 注册为 LVGL 输入设备
 */
int lvgl_input_register(input_driver_t *drv, lv_indev_type_t type)
{
    lv_indev_drv_init(&g_indev_drv);
    g_indev_drv.type      = type;
    g_indev_drv.read_cb   = indev_read_cb;
    g_indev_drv.user_data = drv;

    lv_indev_t *indev = lv_indev_drv_register(&g_indev_drv);
    (void)indev;
    return 0;
}

/**
 * @brief 初始化所有输入设备 — 创建触摸屏并注册到 LVGL
 */
void lvgl_input_init_all(void)
{
    /* 1. 构造并初始化触摸屏驱动 */
    touchscreen_init(&g_touchscreen, "/dev/input/event2");

    if (g_touchscreen.base.init(&g_touchscreen.base) != 0) {
        /* 触摸屏初始化失败不阻塞启动，继续运行（仅显示） */
        return;
    }

    /* 2. 注册到 LVGL 作为 pointer 型输入设备 */
    lvgl_input_register(&g_touchscreen.base, LV_INDEV_TYPE_POINTER);
}
