/**
 * @file    image_service.c
 * @brief   图片服务实现
 *
 * 当前为 Phase 2 桩代码。Phase 8 将实现:
 *   1. image_load()  — 调用 LVGL 内置解码器加载 BMP/JPG/PNG
 *   2. image_preload() — 在后台线程中提前解码图片到缓存
 *   3. LRU 缓存管理  — 控制内存占用
 */

#include "image_service.h"

int image_service_init(void)                 { return 0; }
lv_img_dsc_t *image_load(const char *path)   { (void)path; return NULL; }
void image_preload(const char *path)         { (void)path; }
void image_cancel_preload(const char *path)  { (void)path; }
