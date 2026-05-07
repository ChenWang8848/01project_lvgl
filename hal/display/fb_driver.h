/**
 * @file    fb_driver.h
 * @brief   Linux framebuffer 显示驱动 — display_driver_t 的具体实现
 *
 * 通过 /dev/fb0 访问 framebuffer，使用 mmap 映射显存。
 * flush 回调将 LVGL 绘制缓冲区的像素逐行拷贝到 mmap 区域。
 */

#ifndef FB_DRIVER_H
#define FB_DRIVER_H

#include "display_driver.h"
#include <linux/fb.h>
#include <stdint.h>

/**
 * @brief framebuffer 驱动对象
 *
 * 继承自 display_driver_t (第一个成员为 base)。
 * 通过 fb_driver_init() 初始化 vtable，然后调用 base.init() 打开设备。
 */
typedef struct {
    display_driver_t base;                     /**< 父类 (vtable) */
    int              fd;                       /**< /dev/fb0 文件描述符 */
    uint8_t         *fb_mmap;                  /**< mmap 映射的显存指针 */
    size_t           fb_size;                  /**< 显存大小 (字节) */
    const char      *dev_path;                 /**< 设备路径 */
    struct fb_var_screeninfo vinfo;            /**< 可变屏幕参数 (分辨率/BPP) */
    struct fb_fix_screeninfo finfo;            /**< 固定屏幕参数 (行长度/显存大小) */
} fb_driver_t;

/**
 * @brief 构造 fb_driver 对象 — 填充 vtable，设置设备路径
 * @param self     对象指针
 * @param dev_path framebuffer 设备路径 (如 "/dev/fb0")
 */
void fb_driver_init(fb_driver_t *self, const char *dev_path);

#endif
