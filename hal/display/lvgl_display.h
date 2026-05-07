/**
 * @file    lvgl_display.h
 * @brief   LVGL 显示适配层 — 将 display_driver_t 注册为 LVGL 显示驱动
 *
 * 职责:
 *   1. 从硬件驱动获取分辨率
 *   2. 分配 LVGL draw buffer (全屏单缓冲)
 *   3. 绑定 flush/wait 回调到 LVGL disp_drv
 *   4. 调用 lv_disp_drv_register() 注册显示设备
 */

#ifndef LVGL_DISPLAY_H
#define LVGL_DISPLAY_H

#include "lvgl/lvgl.h"
#include "display_driver.h"

/**
 * @brief LVGL 显示适配器
 *
 * 组合关系: 持有一个 display_driver_t* 指针指向实际硬件驱动。
 * LVGL 调用 flush_cb 时，转发到 hw->flush()。
 */
typedef struct {
    lv_disp_drv_t      drv;         /**< LVGL 显示驱动结构体 */
    lv_disp_draw_buf_t draw_buf;    /**< LVGL 绘制缓冲区描述符 */
    lv_color_t        *buf1;        /**< 绘制缓冲区 (主) */
    lv_color_t        *buf2;        /**< 绘制缓冲区 (辅助, 双缓冲时为非 NULL) */
    display_driver_t  *hw;          /**< 实际硬件驱动 (fb_driver 等) */
    int                hor_res;     /**< 水平分辨率 (缓存) */
    int                ver_res;     /**< 垂直分辨率 (缓存) */
} lvgl_display_t;

/**
 * @brief 初始化 LVGL 显示适配器
 * @param self 适配器对象
 * @param hw   已初始化的硬件驱动 (需先调用 hw->init())
 * @return 0: 成功, -1: 内存分配失败
 */
int  lvgl_display_init(lvgl_display_t *self, display_driver_t *hw);

/**
 * @brief 释放 draw buffer 内存
 */
void lvgl_display_deinit(lvgl_display_t *self);

#endif
