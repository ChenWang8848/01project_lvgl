/**
 * @file    image_cache.h
 * @brief   图片预加载缓存 — 多线程安全, 4槽 LRU
 *
 * 工作线程解码图片为 RGBA8888 像素存入缓存,
 * 主线程翻页时命中缓存则瞬时显示。
 */
#ifndef IMAGE_CACHE_H
#define IMAGE_CACHE_H

#include <stdint.h>

#define CACHE_SLOTS 4

typedef struct {
    uint8_t  *pixels;       /**< 解码后像素数据 (malloc), 可直接用于 lv_img_dsc_t.data */
    uint32_t  w, h;         /**< 图像宽高 */
    uint8_t   cf;           /**< LVGL 颜色格式 (LV_IMG_CF_...) */
    char      path[512];    /**< 源文件路径 (用于匹配) */
    int       ready;        /**< 1=解码完成, 0=解码中 */
} cache_slot_t;

/** 初始化缓存 (所有槽标记为无效) */
void image_cache_init(void);

/** 销毁缓存, 释放所有像素内存 */
void image_cache_destroy(void);

/**
 * @brief 查询缓存中是否已有指定路径的图片
 * @param path  文件路径
 * @param w,h   [out] 图像尺寸
 * @return 像素数据指针 (可直接用于 lv_img_dsc_t), NULL 表示未命中
 */
uint8_t *image_cache_lookup(const char *path, uint32_t *w, uint32_t *h, uint8_t *cf);

/**
 * @brief 将解码后的像素写入缓存 (工作线程调用)
 * @param path   文件路径 (用于匹配)
 * @param pixels 解码像素 (所有权转移给缓存, malloc 分配)
 * @param w,h    图像尺寸
 * @param cf     LVGL 颜色格式
 */
void image_cache_put(const char *path, uint8_t *pixels, uint32_t w, uint32_t h, uint8_t cf);

#endif
