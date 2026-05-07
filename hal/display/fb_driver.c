/**
 * @file    fb_driver.c
 * @brief   Linux framebuffer 驱动实现
 *
 * 工作流程:
 *   1. open("/dev/fb0")          — 打开 framebuffer 设备
 *   2. ioctl(FBIOGET_VSCREENINFO) — 获取分辨率/位深
 *   3. ioctl(FBIOGET_FSCREENINFO) — 获取行长度/显存大小
 *   4. mmap(...)                  — 映射显存到用户空间
 *   5. flush()                    — 将 LVGL 像素拷贝到 mmap 区域
 *
 * 像素格式转换:
 *   LVGL 内部使用 lv_color_t (根据 LV_COLOR_DEPTH 为 16/32 bit)
 *   framebuffer 通常为 32bit ARGB/BGRA 或 16bit RGB565
 */

#include "fb_driver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

/* ================================================================
 *  静态函数声明 (vtable 实现)
 * ================================================================ */
static int  fb_init(display_driver_t *base);
static void fb_get_resolution(display_driver_t *base, int *w, int *h);
static int  fb_get_bpp(display_driver_t *base);
static void fb_flush(display_driver_t *base, lv_disp_drv_t *drv,
                     const lv_area_t *area, lv_color_t *color_p);
static void fb_wait_cb(display_driver_t *base, lv_disp_drv_t *drv);
static void fb_deinit(display_driver_t *base);

/* ================================================================
 *  构造/析构
 * ================================================================ */

/**
 * @brief 初始化 fb_driver 对象 — 填充 vtable，设置默认设备路径
 */
void fb_driver_init(fb_driver_t *self, const char *dev_path)
{
    memset(self, 0, sizeof(*self));

    /* 填充 vtable (函数指针表) */
    self->base.name           = "fbdev";
    self->base.init           = fb_init;
    self->base.get_resolution = fb_get_resolution;
    self->base.get_bpp        = fb_get_bpp;
    self->base.flush          = fb_flush;
    self->base.wait_cb        = fb_wait_cb;
    self->base.deinit         = fb_deinit;

    self->fd       = -1;
    self->dev_path = dev_path ? dev_path : "/dev/fb0";
}

/* ================================================================
 *  vtable 实现
 * ================================================================ */

/**
 * @brief 打开 framebuffer 设备，获取屏幕信息，mmap 显存
 */
static int fb_init(display_driver_t *base)
{
    fb_driver_t *self = (fb_driver_t *)base;

    /* 1. 打开设备 */
    self->fd = open(self->dev_path, O_RDWR);
    if (self->fd < 0) {
        perror("fb_driver: open");
        return -1;
    }

    /* 2. 获取可变屏幕信息 (分辨率、BPP) */
    if (ioctl(self->fd, FBIOGET_VSCREENINFO, &self->vinfo) < 0) {
        perror("fb_driver: FBIOGET_VSCREENINFO");
        close(self->fd);
        return -1;
    }

    /* 3. 获取固定屏幕信息 (行长度、显存大小) */
    if (ioctl(self->fd, FBIOGET_FSCREENINFO, &self->finfo) < 0) {
        perror("fb_driver: FBIOGET_FSCREENINFO");
        close(self->fd);
        return -1;
    }

    /* 4. mmap 映射显存 */
    self->fb_size = self->finfo.smem_len;
    self->fb_mmap = (uint8_t *)mmap(NULL, self->fb_size,
                                     PROT_READ | PROT_WRITE,
                                     MAP_SHARED, self->fd, 0);
    if (self->fb_mmap == MAP_FAILED) {
        perror("fb_driver: mmap");
        close(self->fd);
        return -1;
    }

    printf("fb_driver: %dx%d, bpp=%d, mem=%zu bytes\n",
           self->vinfo.xres, self->vinfo.yres,
           self->vinfo.bits_per_pixel, self->fb_size);

    return 0;
}

/**
 * @brief 获取屏幕分辨率
 */
static void fb_get_resolution(display_driver_t *base, int *w, int *h)
{
    fb_driver_t *self = (fb_driver_t *)base;
    *w = self->vinfo.xres;
    *h = self->vinfo.yres;
}

/**
 * @brief 获取像素位深
 */
static int fb_get_bpp(display_driver_t *base)
{
    fb_driver_t *self = (fb_driver_t *)base;
    return self->vinfo.bits_per_pixel;
}

/**
 * @brief LVGL flush 回调 — 将脏区域的像素拷贝到 framebuffer
 *
 * 逐行拷贝像素数据，按 framebuffer 的 line_length 计算行偏移。
 * 支持 32bit (ARGB) 和 16bit (RGB565) 两种色深。
 */
static void fb_flush(display_driver_t *base, lv_disp_drv_t *drv,
                     const lv_area_t *area, lv_color_t *color_p)
{
    fb_driver_t *self = (fb_driver_t *)base;
    (void)drv;

    if (!self->fb_mmap) {
        lv_disp_flush_ready(drv);
        return;
    }

    uint32_t xres = self->vinfo.xres;
    uint32_t yres = self->vinfo.yres;
    uint32_t bpp  = self->vinfo.bits_per_pixel;

    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);

    /* 边界裁剪 (防止越界写显存) */
    if (area->x1 >= (int32_t)xres || area->y1 >= (int32_t)yres) {
        lv_disp_flush_ready(drv);
        return;
    }
    if (area->x1 + w > (int32_t)xres) w = xres - area->x1;
    if (area->y1 + h > (int32_t)yres) h = yres - area->y1;

    /* 逐行拷贝像素 */
    for (int32_t y = 0; y < h; y++) {
        uint32_t fb_y = (area->y1 + y) * self->finfo.line_length;
        for (int32_t x = 0; x < w; x++) {
            lv_color_t c = color_p[y * w + x];
            uint32_t off = fb_y + (area->x1 + x) * (bpp / 8);

#if LV_COLOR_DEPTH == 32
            /* 32bit 色深: LVGL 内部为 ARGB8888, framebuffer 通常为 BGRA */
            self->fb_mmap[off + 0] = c.ch.blue;
            self->fb_mmap[off + 1] = c.ch.green;
            self->fb_mmap[off + 2] = c.ch.red;
            self->fb_mmap[off + 3] = 0xFF;
#elif LV_COLOR_DEPTH == 16
            /* 16bit 色深: RGB565 */
            uint16_t *p = (uint16_t *)(self->fb_mmap + off);
            *p = c.full;
#endif
        }
    }

    /* 通知 LVGL flush 完成 */
    lv_disp_flush_ready(drv);
}

/**
 * @brief 等待垂直同步 (当前为空实现)
 */
static void fb_wait_cb(display_driver_t *base, lv_disp_drv_t *drv)
{
    (void)base;
    (void)drv;
}

/**
 * @brief 释放资源 — munmap 显存, close 设备
 */
static void fb_deinit(display_driver_t *base)
{
    fb_driver_t *self = (fb_driver_t *)base;
    if (self->fb_mmap && self->fb_mmap != MAP_FAILED)
        munmap(self->fb_mmap, self->fb_size);
    if (self->fd >= 0)
        close(self->fd);
    self->fb_mmap = NULL;
    self->fd = -1;
}
