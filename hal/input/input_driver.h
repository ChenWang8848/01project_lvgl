/**
 * @file    input_driver.h
 * @brief   输入设备驱动虚基类 — 定义触摸屏/鼠标等输入设备的统一接口
 *
 * 设计模式: 函数指针表 (vtable) 实现运行时多态。
 * LVGL indev 回调通过 read() 方法获取输入数据。
 */

#ifndef INPUT_DRIVER_H
#define INPUT_DRIVER_H

#include "lvgl/lvgl.h"
#include <stdbool.h>

typedef struct input_driver_s input_driver_t;

/**
 * @brief 输入设备驱动虚基类
 */
struct input_driver_s {
    const char *name;           /**< 驱动名称 ("touchscreen" / "mouse") */

    /** @brief 初始化设备 (打开设备节点) */
    int  (*init)(input_driver_t *self);

    /**
     * @brief 读取输入数据 — LVGL indev read 回调
     * @param drv   LVGL indev driver
     * @param data  [out] 输入数据 (坐标/按键状态)
     * @return true: 有数据更新, false: 无数据
     */
    bool (*read)(input_driver_t *self, lv_indev_drv_t *drv, lv_indev_data_t *data);

    /** @brief 关闭设备, 释放资源 */
    void (*deinit)(input_driver_t *self);
};

#endif
