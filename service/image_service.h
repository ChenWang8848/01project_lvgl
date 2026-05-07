/**
 * @file    image_service.h
 * @brief   图片服务 — 图片加载、预加载、缓存管理
 *
 * Phase 8 将实现后台预加载线程和缓存淘汰策略。
 */

#ifndef IMAGE_SERVICE_H
#define IMAGE_SERVICE_H

#include "lvgl/lvgl.h"

/** @brief 初始化图片服务 (启动预加载线程) */
int image_service_init(void);

/** @brief 同步加载图片，返回 LVGL 图像描述符 */
lv_img_dsc_t *image_load(const char *path);

/** @brief 后台预加载图片到缓存 */
void image_preload(const char *path);

/** @brief 取消后台预加载 */
void image_cancel_preload(const char *path);

#endif
