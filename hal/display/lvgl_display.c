/**
 * @file    lvgl_display.c
 * @brief   LVGL 显示适配层实现
 *
 * 连接 LVGL 框架与硬件显示驱动:
 *   LVGL 绘制引擎  →  draw buffer  →  flush 回调  →  hw->flush()  →  framebuffer
 *
 * LVGL v8 flush 回调协议:
 *   LVGL 调用 flush_cb(drv, area, color_p) 通知驱动将指定区域
 *   的像素刷新到屏幕。驱动完成后必须调用 lv_disp_flush_ready(drv)。
 */

#include "lvgl_display.h"
#include <stdlib.h>
#include <string.h>

/* ================================================================
 *  静态函数 — LVGL 回调
 * ================================================================ */

/**
 * @brief LVGL flush 回调 — 转发到硬件驱动的 flush 方法
 *
 * LVGL 在完成一帧渲染后调用此函数，将脏区域的像素数据 color_p
 * 传递给硬件驱动进行显示刷新。
 */
static void disp_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area,
                          lv_color_t *color_p)
{
    lvgl_display_t *self = (lvgl_display_t *)drv->user_data;
    if (self->hw && self->hw->flush)
        self->hw->flush(self->hw, drv, area, color_p);
    else
        lv_disp_flush_ready(drv);
}

/**
 * @brief LVGL wait_cb 回调 — 转发到硬件驱动的 wait_cb 方法
 *
 * 用于等待垂直同步信号，避免画面撕裂。
 * 当前为空实现 (不需要等待 vsync)。
 */
static void disp_wait_cb(lv_disp_drv_t *drv)
{
    lvgl_display_t *self = (lvgl_display_t *)drv->user_data;
    if (self->hw && self->hw->wait_cb)
        self->hw->wait_cb(self->hw, drv);
}

/* ================================================================
 *  公开接口
 * ================================================================ */

/**
 * @brief 创建并注册 LVGL 显示驱动
 *
 * 步骤:
 *   1. 从硬件驱动获取分辨率
 *   2. 分配全屏 draw buffer (malloc)
 *   3. 初始化 lv_disp_draw_buf 和 lv_disp_drv
 *   4. 注册到 LVGL (lv_disp_drv_register)
 */
int lvgl_display_init(lvgl_display_t *self, display_driver_t *hw)
{
    memset(self, 0, sizeof(*self));
    self->hw = hw;

    /* 从硬件驱动获取分辨率 (兜底默认 800x480) */
    int w = 800, h = 480;
    if (hw && hw->get_resolution)
        hw->get_resolution(hw, &w, &h);
    self->hor_res = w;
    self->ver_res = h;

    /* 分配 LVGL draw buffer (全屏单缓冲) */
    uint32_t buf_size = w * h;
    self->buf1 = (lv_color_t *)malloc(buf_size * sizeof(lv_color_t));
    if (!self->buf1)
        return -1;

    /* 初始化 draw buffer 描述符 */
    lv_disp_draw_buf_init(&self->draw_buf, self->buf1, self->buf2, buf_size);

    /* 初始化显示驱动并注册 */
    lv_disp_drv_init(&self->drv);
    self->drv.draw_buf   = &self->draw_buf;
    self->drv.flush_cb   = disp_flush_cb;
    self->drv.wait_cb    = disp_wait_cb;
    self->drv.hor_res    = w;
    self->drv.ver_res    = h;
    self->drv.user_data  = self;      /* 回调中通过 drv->user_data 取回 self */

    lv_disp_t *disp = lv_disp_drv_register(&self->drv);
    (void)disp;

    return 0;
}

/**
 * @brief 释放 draw buffer
 */
void lvgl_display_deinit(lvgl_display_t *self)
{
    if (self->buf1) {
        free(self->buf1);
        self->buf1 = NULL;
    }
}
