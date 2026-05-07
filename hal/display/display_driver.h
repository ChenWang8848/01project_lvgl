/**
 * @file    display_driver.h
 * @brief   显示驱动虚基类 — 定义 framebuffer/DRM 等显示后端的统一接口
 *
 * 设计模式: 使用函数指针表 (vtable) 实现运行时多态。
 * 子类 (fb_driver_t) 将第一个成员设为本结构体，实现"继承"。
 * 所有函数指针由子类在 init 阶段填充。
 */

#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include "lvgl/lvgl.h"

typedef struct display_driver_s display_driver_t;

/**
 * @brief 显示驱动虚基类 (vtable)
 */
struct display_driver_s {
    const char *name;           /**< 驱动名称 (e.g. "fbdev", "drm") */

    /**
     * @brief  初始化硬件 (打开设备、获取分辨率等)
     * @return 0: 成功, -1: 失败
     */
    int  (*init)(display_driver_t *self);

    /**
     * @brief 获取屏幕分辨率
     * @param w  [out] 水平像素
     * @param h  [out] 垂直像素
     */
    void (*get_resolution)(display_driver_t *self, int *w, int *h);

    /**
     * @brief 获取像素位深 (bits per pixel)
     */
    int  (*get_bpp)(display_driver_t *self);

    /**
     * @brief LVGL flush 回调 — 将像素块刷新到显示设备
     * @param drv     LVGL display driver
     * @param area    脏区域 (需要更新的矩形)
     * @param color_p 像素数据指针
     *
     * 实现要点:
     *   1. 将 color_p 中的像素拷贝到显存 (如 fb mmap)
     *   2. 拷贝完成后必须调用 lv_disp_flush_ready(drv)
     */
    void (*flush)(display_driver_t *self, lv_disp_drv_t *drv,
                  const lv_area_t *area, lv_color_t *color_p);

    /**
     * @brief LVGL wait_cb 回调 — 等待垂直同步 (可选)
     */
    void (*wait_cb)(display_driver_t *self, lv_disp_drv_t *drv);

    /**
     * @brief 释放硬件资源 (munmap, close fd 等)
     */
    void (*deinit)(display_driver_t *self);
};

#endif
